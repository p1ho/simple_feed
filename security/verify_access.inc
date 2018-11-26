<?php

/**
 * Functions that verify user access
 */

/**
* Check if parameters passed by client passes.
* Key set in http header takes precendence over query string, and if the former
* is wrong, even if the one in query string is correct, it will deny access.
* @return boolean
*/
function simple_feed_verify_access($profile) {
  module_load_include('inc', 'simple_feed', 'resources/http_send');
  module_load_include('inc', 'simple_feed', 'resources/db_control');
  module_load_include('inc', 'simple_feed');

  $profile_dict = simple_feed_get_profile($profile);

  $params = drupal_get_query_parameters();

  // check referer
  if ($profile_dict['http_referer_restrict']) {
    if (!simple_feed_check_referer($profile_dict)) {
       simple_feed_send_access_denied();
    }
  }

  // check ip
  if ($profile_dict['ip_address_restrict']) {
    if (!simple_feed_check_ip_address($profile_dict)) {
      simple_feed_send_access_denied();
    }
  }

  // check key
  if (!simple_feed_check_key($profile_dict)) {
    simple_feed_send_access_denied();
  }

  return TRUE;
}

/**
 * simple_feed_check_referer.
 * Checks if incoming request has HTTP referers, if so, pass it to the
 * simple_feed_verify_referer helper function with the array of white-listed
 * referers. Will return TRUE if referer test passes.
 * @param  Array  $profile_dict [profile settings]
 * @return boolean
 */
function simple_feed_check_referer($profile_dict) {
  if (isset($_SERVER["HTTP_REFERER"]) && !empty($_SERVER["HTTP_REFERER"])) {
    $referers_allowed = json_decode($profile_dict['http_referers_allowed'], TRUE);
    return simple_feed_verify_referer($_SERVER["HTTP_REFERER"], $referers_allowed);
  } else {
    return TRUE;
  }
}

/**
 * simple_feed_verify_referer function.
 * Because we will also allow refers from subdomain of a website, we run through
 * the white-listed domains and check if the beginnings are the same.
 * @param  String $referer          [incoming referer]
 * @param  Array  $referers_allowed [referers allowed for this profile]
 * @return boolean
 */
function simple_feed_verify_referer($referer, $referers_allowed) {
  foreach ($referers_allowed as $referer_allowed) {
    if (substr($referer, 0 , strlen($referer_allowed)) === $referer_allowed) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * simple_feed_check_ip_address.
 * Checks IP.
 * @param  Array  $profile_dict [profile settings]
 * @return boolean
 */
function simple_feed_check_ip_address($profile_dict) {
  $ip_addresses_allowed = json_decode($profile_dict['ip_addresses_allowed'], TRUE);
  if (isset($_SERVER["REMOTE_ADDR"])) {
    $remote_addr = $_SERVER["REMOTE_ADDR"];
    // there is an edge case where a server may act as a proxy for a user to
    // make another request to itself, rendering REMOTE_ADDR as local.
    // For this scenario, we must check the IP that's being forwarded.
    if ($remote_addr === $_SERVER["SERVER_ADDR"]) {
      if (isset($_SERVER["HTTP_X_FORWARDED_FOR"])){
        $forwarded_ips = explode(',', $_SERVER["HTTP_X_FORWARDED_FOR"]);
        $forwarded_ip_first = trim($forwarded_ips[0]); // original requester
        $forwarded_ip_last = trim($forwarded_ips[count($forwarded_ips)-1]); // most recent proxy
        // if most recent proxy's ip is not the same as remote_addr, then we
        // suspect IP has been spoofed and return access denied
        return ($forwarded_ip_last === $remote_addr && in_array($forwarded_ip_first, $ip_addresses_allowed));
      } else {
        // If HTTP_X_FORWARDED_FOR does not exist, it means there has been no
        // redirect and the request is actually coming locally. We grant access in
        // this case.
        return TRUE;
      }
    } else {
      return in_array($remote_addr, $ip_addresses_allowed);
    }
  } else {
    return FALSE; // if REMOTE_ADDR is not set, we deny access
  }
}

/**
 * simple_feed_check_key.
 * Checks access-key. We first check if it's appended in the HTTP header (which
 * is our intended way of authorizing), if so, check if it's correct. If not,
 * we check if the setting to allow key appended in the query string is enabled;
 * if so, we check the key; if not we fail the test even if the key is correct.
 * @param  Array  $profile_dict [profile settings]
 * @return boolean
 */
function simple_feed_check_key($profile_dict) {
  $true_key = $profile_dict['access_key'];
  // Not using conventional HTTP_AUTHORIZATION for special cases where
  // the Authorization header may be trimmed on redirect.
  if (isset($_SERVER["HTTP_FEED_AUTHORIZATION"]) && !empty($_SERVER["HTTP_FEED_AUTHORIZATION"])) {
    $auth_header = $_SERVER["HTTP_FEED_AUTHORIZATION"];
    if (substr($auth_header, 0, 4) === "KEY=") {
      $key = substr($auth_header, 4);
      return $key === $true_key;
    } else {
      return FALSE;
    }
  } else {
    $params = drupal_get_query_parameters();
    if ($profile_dict['access_key_allow_in_url'] && isset($params["key"])) {
      return $params["key"] === $true_key;
    } else {
      return FALSE;
    }
  }
}

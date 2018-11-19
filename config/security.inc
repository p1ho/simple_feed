<?php

/**
 * Security part of the config form.
 */
function simple_feed_get_form_security($profile_dict) {
  $form = array();

  // Set Tab Container
  $form['security'] = array(
    '#type' => 'fieldset',
    '#title' => t('Security settings'),
    '#group' => 'vertical_tabs',
  );

  // Set Access Key
  $form['security']['access_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Access Key'),
    '#default_value' => $profile_dict['access_key'],
    '#required' => TRUE,
  );
  $form['security']['access_key_allow_in_url'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Allow access key to be entered in the URL'),
    '#options' => drupal_map_assoc(array('Enable')),
    '#default_value' => simple_feed_boolean_to_form_selections($profile_dict['access_key_allow_in_url']==1, 'Enable'),
  );

  // Setting for HTTP referer
  // Set Whether restrict HTTP referer
  $form['security']['http_referer_restrict'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enable HTTP referer Restriction'),
    '#options' => drupal_map_assoc(array('Enable')),
    '#default_value' => simple_feed_boolean_to_form_selections($profile_dict['http_referer_restrict']==1, 'Enable'),
  );
  // Enter new allowed HTTP referer
  $form['security']['http_referers_allowed'] = array(
    '#type' => 'textarea',
    '#title' => t('Enter allowed HTTP referers'),
    '#description' => t("Sub-directories will also be granted access.").'<br>'.t("Separate entries by newline."),
    '#states' => array(
      'disabled' => array(
        ':input[name="http_referer_restrict[Enable]"]' => array('checked' => FALSE),
      )
    ),
    // http://avadhutp.github.io/line-breaks-in-textarea-default-value-in-drupal-7/
    '#default_value' => html_entity_decode(implode('&#13;&#10;', json_decode($profile_dict['http_referers_allowed'], TRUE))),
  );

  // Setting for IP Address
  // Set Whether restrict IP Address
  $form['security']['ip_address_restrict'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Enable IP Address Restriction'),
    '#options' => drupal_map_assoc(array('Enable')),
    '#default_value' => simple_feed_boolean_to_form_selections($profile_dict['ip_address_restrict']==1, 'Enable'),
  );
  // List of allowed HTTP referers (Subdomains are allowed)
  $form['security']['ip_addresses_allowed'] = array(
    '#type' => 'textarea',
    '#title' => t('Set allowed IP Addresses'),
    '#description' => t("Separate entries by newline."),
    '#states' => array(
      'disabled' => array(
        ':input[name="ip_address_restrict[Enable]"]' => array('checked' => FALSE),
      )
    ),
    // http://avadhutp.github.io/line-breaks-in-textarea-default-value-in-drupal-7/
    '#default_value' => html_entity_decode(implode('&#13;&#10;', json_decode($profile_dict['ip_addresses_allowed'], TRUE))),
  );

  return $form;

}

/**
 * sanitize and validate entered http referers and ip addresses.
 */
function simple_feed_security_validate(&$form, &$form_state) {
  // Validate http referers, trim off bad entries
  if (isset($form_state['values']['http_referers_allowed'])) {
    $response = $form_state['values']['http_referers_allowed'];
    if (trim($response) === "") {
      $form_state['values']['http_referers_allowed'] = array();
    } else {
      $http_referers = explode("\r\n", $response);
      $http_referers_length = count($http_referers);
      for ($i = 0; $i < $http_referers_length; $i++) {
        if (!preg_match(simple_feed_get_valid_url_regexp(), $http_referers[$i])) {
          drupal_set_message(t('@referer is not a valid http referer, entry removed.', array('@referer' => '"'.$http_referers[$i].'"')), 'warning');
          unset($http_referers[$i]);
        }
      }
      $http_referers = array_values($http_referers);
      $form_state['values']['http_referers_allowed'] = $http_referers;
    }
  }
  // Validate ip addresses, trim off bad entries
  if (isset($form_state['values']['ip_addresses_allowed'])) {
    $response = $form_state['values']['ip_addresses_allowed'];
    if (trim($response) === "") {
      $form_state['values']['ip_addresses_allowed'] = array();
    } else {
      $ip_addresses = explode("\r\n", $response);
      $ip_addresses_length = count($ip_addresses);
      for ($i = 0; $i < $ip_addresses_length; $i++) {
        if (!filter_var($ip_addresses[$i], FILTER_VALIDATE_IP)) {
          drupal_set_message(t('@ip is not a valid ip address, entry removed.', array('@ip' => '"'.$ip_addresses[$i].'"')), 'warning');
          unset($ip_addresses[$i]);
        }
      }
      $ip_addresses = array_values($ip_addresses);
      $form_state['values']['ip_addresses_allowed'] = $ip_addresses;
    }
  }
}

/**
 * https://mathiasbynens.be/demo/url-regex
 */
function simple_feed_get_valid_url_regexp() {
  return  '%^(?:(?:https?|ftp)://)(?:\S+(?::\S*)?@|\d{1,3}(?:\.\d{1,3}){3}|(?:(?:[a-z\d\x{00a1}-\x{ffff}]+-?)*[a-z\d\x{00a1}-\x{ffff}]+)(?:\.(?:[a-z\d\x{00a1}-\x{ffff}]+-?)*[a-z\d\x{00a1}-\x{ffff}]+)*(?:\.[a-z\x{00a1}-\x{ffff}]{2,6}))(?::\d+)?(?:[^\s]*)?$%iu';
}

<?php

/**
 * Builds a more informative package
 * @param  String|Integer $status [http status code]
 * @param  Array          $pkg    [queried package]
 * @param  Array          $query_summary [summary of queried package]
 * @param  String         $suggested_action [any suggested action for user]
 * @return array         [Completed Package]
 */
function simple_feed_build_package($status, $pkg = array(), $query_summary = array(), $suggested_action = "") {
  $package = array(
    "status" => (string)$status,
    "message" => simple_feed_get_http_msg($status),
  );
  if (count($query_summary) !== 0) {
    $package["query_summary"] = $query_summary;
  }
  if (count($pkg) !== 0) {
    $package["result"] = $pkg;
  }
  if ($suggested_action !== "") {
    $package["suggested_action"] = $suggested_action;
  }
  return $package;
}

/**
 * send http response and exit
 * @param  Array $pkg  [Array to be converted to json and sent to client]
 * @return void
 */
function simple_feed_send_json($pkg) {
  $GLOBALS['devel_shutdown'] = FALSE; // suppressing devel output
  http_response_code($pkg["status"]);
  header('Content-Type: application/json');
  echo json_encode($pkg);
  exit(); // if I use drupal_exit(), authorization fail package will send twice for some rason.
}

function simple_feed_send_not_available($query_summary) {
  $pkg = simple_feed_build_package(404, array(), $query_summary, "Requested data not available. Please try other query parameters.");
  simple_feed_send_json($pkg);
}

function simple_feed_send_invalid_input($query_summary) {
  $pkg = simple_feed_build_package(400, array(), $query_summary, "Invalid Input: a numeric ID is expected in the URL.");
  simple_feed_send_json($pkg);
}

function simple_feed_send_invalid_inputs($query_summary) {
  $pkg = simple_feed_build_package(400, array(), $query_summary, "Invalid Input: one or more numeric IDs (comma delimited) are expected in the URL (Or in the body if this is sent in a POST request).");
  simple_feed_send_json($pkg);
}

function simple_feed_send_access_denied() {
  $pkg = simple_feed_build_package(403, array(), array(), "Authorization failed: please double check your access key "
                                            . "(append it to the HTTP request header 'Authorization: KEY=your-access-key', "
                                            . "or try appending the access key to the query parameters under 'key'). "
                                            . "If you're sure your access key is correct, please contact system administrator.");
  simple_feed_send_json($pkg);
}

/**
 * Return an appropriate message for the status code passed
 * @param  String|Integer $status [an int or string representing http code]
 * @return String         Message
 */
function simple_feed_get_http_msg($status) {
  $status = (string)$status;
  $http_codes = array(
    "200" => "OK: Request has succeeded.",
    "201" => "Created: Request has been fulfilled, new resource created.",
    "202" => "Accepted: Request has been accepted for processing.",
    "400" => "Bad Request: Request could not be understood by the server.",
    "401" => "Unauthorized: Request requires user authentication.",
    "403" => "Forbidden: Request was understood but the information requested is forbidden.",
    "404" => "Not Found: No matching information was found for the query.",
    "409" => "Conflict: Request could not be completed due to a conflict with the resource's current state.",
    "500" => "Internal Server Error: Server encountered unexpected error.",
    "503" => "Service Unavailable: Unable to handle request due to overloading or maintenance.",
  );
  if (isset($http_codes[$status])) {
    return $http_codes[$status];
  } else {
    throw new Exception(t("Status code passed was not recognized."));
  }
}

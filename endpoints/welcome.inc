<?php

/**
 * Send profile welcome json
 */
function simple_feed_get_welcome_profile($profile, $endpoint) {
  module_load_include('inc', 'simple_feed', 'resources/http_send');
  $endpoints = array(
    '/summary',
    '/node',
    '/nodes',
    '/taxonomy_term',
    '/taxonomy_terms',
  );
  if (!in_array("/$endpoint", $endpoints)) {
    $pkg = simple_feed_build_package(
      404,
      "Endpoint /$endpoint does not exist.",
      array(),
      "Available endpoints: " . implode(', ', $endpoints)
    );
  } else {
    $pkg = simple_feed_build_package(
      200,
      "Profile $profile is set up correctly.",
      array(),
      "Available endpoints: " . implode(', ', $endpoints)
    );
  }
  simple_feed_send_json($pkg);
}

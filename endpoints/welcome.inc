<?php

/**
 * Send profile welcome json
 */
function simple_feed_get_welcome_profile($profile) {
  module_load_include('inc', 'simple_feed', 'resources/http_send');
  $pkg = simple_feed_build_package(
    200,
    "Profile $profile is set up correctly.",
    array(),
    'Available endpoints: /summary, /node, /nodes, /taxonomy_term, /taxonomy_terms'
  );
  simple_feed_send_json($pkg);
}

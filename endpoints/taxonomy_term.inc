<?php

/**
 * Send taxonomy term json
 */
function simple_feed_get_taxonomy_term($profile, $id=NULL) {
  module_load_include('inc', 'simple_feed', 'resources/http_send');
  module_load_include('inc', 'simple_feed', 'resources/db_control');

  $profile_dict = simple_feed_get_profile($profile);

  $query_summary = array(
    "requested" => $id,
    "returned" => "",
    "result_count" => 0,
  );
  if ($id === NULL || !is_numeric($id)) {
    simple_feed_send_invalid_input($query_summary);
  } else {
    module_load_include('inc', 'simple_feed');
    $taxonomy_vocabs_allowed = json_decode($profile_dict['taxonomy_vocabs_allowed'], TRUE);
    $taxonomy_vocabs_id = simple_feed_taxonomy_vocabulary_get_selected_id($taxonomy_vocabs_allowed);
    $taxonomy_term_attr = json_decode($profile_dict['taxonomy_term_attributes'], TRUE);

    $entity_type = "taxonomy_term";
    $entity = simple_feed_entity_load_with_url($entity_type, $id);

    if ($entity !== FALSE) {
      // test if vocab type is allowed
      if (!in_array($entity->vid, $taxonomy_vocabs_id)) {
        $entity = FALSE;
      }
    }
    if ($entity === FALSE) {
      simple_feed_send_not_available($query_summary);
    } else {
      $query_summary["returned"] = $entity->tid;
      $query_summary["result_count"] = 1;
      $pkg_result = simple_feed_trim_entity_attr(array($entity), $taxonomy_term_attr)[0];
      $pkg = simple_feed_build_package(200, $pkg_result, $query_summary);
      simple_feed_send_json($pkg);
    }
  }
}

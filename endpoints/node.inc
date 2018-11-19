<?php

/**
 * Send node json
 */
function simple_feed_get_node($profile, $id=NULL) {
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
    $node_types = json_decode($profile_dict['node_types_allowed'], TRUE);
    $node_attr = json_decode($profile_dict['node_attributes'], TRUE);

    $entity_type = "node";
    $entity = simple_feed_entity_load_with_url($entity_type, $id);
    if ($entity !== FALSE) {
      // test if node type is allowed
      if (!in_array($entity->type, $node_types)) {
        $entity = FALSE;
      } else {
        // test if unpublished is allowed
        if (!$profile_dict['allow_unpublished'] && $entity->status == 0) {
          $entity = FALSE;
        }
      }
    }
    if ($entity === FALSE) {
      simple_feed_send_not_available($query_summary);
    } else {
      $query_summary["returned"] = (string)$entity->nid;
      $query_summary["result_count"] = 1;
      $pkg_result = simple_feed_trim_entity_attr(array($entity), $node_attr)[0];
      $pkg = simple_feed_build_package(200, $pkg_result, $query_summary);
      simple_feed_send_json($pkg);
    }
  }
}

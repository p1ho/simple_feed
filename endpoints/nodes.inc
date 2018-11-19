<?php

/**
 * Send nodes listing json
 */
function simple_feed_get_nodes($profile, $ids_str="") {
  module_load_include('inc', 'simple_feed', 'resources/http_send');
  module_load_include('inc', 'simple_feed', 'resources/db_control');
  module_load_include('inc', 'simple_feed');

  $profile_dict = simple_feed_get_profile($profile);

  $ids = simple_feed_parse_ids_str($ids_str);
  $query_summary = array(
    "requested" => $ids,
    "returned" => array(),
    "result_count" => 0,
  );
  if (count($ids) === 0) {
    simple_feed_send_invalid_inputs($query_summary);
  } else {
    $node_types = json_decode($profile_dict['node_types_allowed'], TRUE);
    $node_attr = json_decode($profile_dict['node_attributes'], TRUE);

    $entity_type = "node";
    if (!$profile_dict['allow_unpublished']) {
      $query = new EntityFieldQuery();
      $query->entityCondition('entity_type', $entity_type)
            ->entityCondition('bundle', $node_types)
            ->propertyCondition('nid', $ids)
            ->propertyCondition('status', NODE_PUBLISHED);
      $result = $query->execute();
      $ids_pub = (isset($result[$entity_type])) ? array_keys($result[$entity_type]) : array();
      $entities = simple_feed_entity_load_multiple_with_url($entity_type, $ids_pub);
    } else {
      $entities = simple_feed_entity_load_multiple_with_url($entity_type, $ids);
    }
    ksort($entities); // reorder array

    if (count($result) === 0) {
      simple_feed_send_not_available($query_summary);
    } else {
      $query_summary["returned"] = array_map("strval",array_keys($entities));
      $query_summary["result_count"] = count($entities);
      $pkg_result = simple_feed_trim_entity_attr($entities, $node_attr);
      $pkg = simple_feed_build_package(200, $pkg_result, $query_summary);
      simple_feed_send_json($pkg);
    }
  }
}

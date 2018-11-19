<?php

/**
 * Send taxonomy term listing json
 */
function simple_feed_get_taxonomy_terms($profile, $ids_str="") {
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
    $taxonomy_vocabs_allowed = json_decode($profile_dict['taxonomy_vocabs_allowed'], TRUE);
    $taxonomy_vocabs_id = simple_feed_taxonomy_vocabulary_get_selected_id($taxonomy_vocabs_allowed);
    $taxonomy_term_attr = json_decode($profile_dict['taxonomy_term_attributes'], TRUE);

    $entity_type = "taxonomy_term";
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $entity_type)
          ->propertyCondition('vid', $taxonomy_vocabs_id);
    $result = $query->execute();
    $ids_allowed = (isset($result[$entity_type])) ? array_keys($result[$entity_type]) : array();
    $entities = simple_feed_entity_load_multiple_with_url($entity_type, $ids_allowed);
    ksort($entities); // reorder array

    if (count($entities) === 0) {
      simple_feed_send_not_available($query_summary);
    } else {
      $query_summary["returned"] = array_map("strval",array_keys($entities));
      $query_summary["result_count"] = count($entities);
      $pkg_result = simple_feed_trim_entity_attr($entities, $taxonomy_term_attr);
      $pkg = simple_feed_build_package(200, $pkg_result, $query_summary);
      simple_feed_send_json($pkg);
    }
  }
}

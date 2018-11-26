<?php

/**
 * Send summary listing json
 */
function simple_feed_get_summary($profile) {
  module_load_include('inc', 'simple_feed', 'resources/http_send');
  module_load_include('inc', 'simple_feed', 'resources/db_control');
  module_load_include('inc', 'simple_feed');

  $profile_dict = simple_feed_get_profile($profile);

  // get all node info, and take out those that we need
  // unfortunately, the fields can't be selected in the query, and must be chosen after fetch
  $node_types = json_decode($profile_dict['node_types_allowed'], TRUE);
  $node_attr = json_decode($profile_dict['summary_node_attributes'], TRUE);

  if (count($node_types) !== 0) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'node')
          ->propertyCondition('type', $node_types);
    if (!$profile_dict['allow_unpublished']) {
      $query->propertyCondition('status', NODE_PUBLISHED);
    }
    $result = $query->execute();

    $id_array = (isset($result['node'])) ? array_keys($result['node']) : array();
    $nodes = simple_feed_entity_load_multiple_with_url('node', $id_array);
    ksort($nodes); // reorder array
  } else {
    $nodes = array();
  }

  // get all taxonomy info, and take out those that we need
  $taxonomy_vocabs_allowed = json_decode($profile_dict['taxonomy_vocabs_allowed'], TRUE);
  $taxonomy_vocabs_id = simple_feed_taxonomy_vocabulary_get_selected_id($taxonomy_vocabs_allowed);
  $taxonomy_term_attr = json_decode($profile_dict['summary_taxonomy_term_attributes'], TRUE);

  if (count($taxonomy_vocabs_id) !== 0) {
    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', 'taxonomy_term')
          ->propertyCondition('vid', $taxonomy_vocabs_id);
    $result = $query->execute();

    $id_array = (isset($result['taxonomy_term'])) ? array_keys($result['taxonomy_term']) : array();
    $taxonomy_terms = simple_feed_entity_load_multiple_with_url('taxonomy_term', $id_array);
    ksort($taxonomy_terms); // reorder array
  } else {
    $taxonomy_terms = array();
  }

  $pkg_result = array(
    "nodes" => simple_feed_trim_entity_attr($nodes, $node_attr),
    "taxonomy_terms" => simple_feed_trim_entity_attr($taxonomy_terms, $taxonomy_term_attr),
  );
  // send http response and exit
  $pkg = simple_feed_build_package(200, $pkg_result);
  simple_feed_send_json($pkg);
}

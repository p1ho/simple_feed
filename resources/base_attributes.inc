<?php

// Node base attributes
// (consult https://www.drupal.org/node/2360907)
function simple_feed_get_base_node_attr() {
  $node_info = entity_get_info('node');
  $base_node_entity_keys = array_values($node_info['entity keys']);
  $base_node_fields = $node_info['schema_fields_sql'];
  $base_node_attr = array_unique(array_merge(
    $base_node_entity_keys,
    $base_node_fields['base table'],
    $base_node_fields['revision table'],
    array('url_original', 'url_aliases')
  ));
  return $base_node_attr;
}

// Taxonomy term base attribute
// (consult https://www.drupal.org/node/2360885)
function simple_feed_get_base_taxonomy_term_attr() {
  $base_taxonomy_term_attr = array(
    'tid',
    'vid',
    'name',
    'description',
    'format',
    'weight',
    'vocabulary_machine_name',
    'url_original',
    'url_aliases',
  );
  return $base_taxonomy_term_attr;
}

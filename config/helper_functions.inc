<?php

/**
 * Helper functions for Config Form
 */

function simple_feed_get_node_attr_options() {
  // load resource for displaying entity contained_by and category
  $base_node_attr = variable_get('simple_feed_base_node_attributes');
  $base_node_attr_flip = array_flip($base_node_attr);
  $field_map = field_info_field_map();
  $node_misc_attr_dict = simple_feed_get_node_misc_attr_dict();
  $filtered_node_attr = simple_feed_get_filtered_node_attr();

  // Build Options
  $node_attributes_options = array();
  foreach ($filtered_node_attr as $machine_name) {
    $node_attributes_options[$machine_name] = array();
    $node_attributes_options[$machine_name]['machine_name'] = $machine_name;

    if (isset($base_node_attr_flip[$machine_name])) {
      $node_attributes_options[$machine_name]['contained_by'] = t('All nodes');
      $node_attributes_options[$machine_name]['category'] = t('Base attributes');
    } else if (isset($field_map[$machine_name]['bundles']['node'])) {
      $node_attributes_options[$machine_name]['contained_by'] = t(implode(', ', $field_map[$machine_name]['bundles']['node']));
      $node_attributes_options[$machine_name]['category'] = t('Custom attributes');
    } else {
      $node_attributes_options[$machine_name]['contained_by'] = t(implode(', ', $node_misc_attr_dict[$machine_name]));
      $node_attributes_options[$machine_name]['category'] = t('Miscellaneous attributes');
    }
  }

  return $node_attributes_options;
}

function simple_feed_get_taxonomy_term_attr_options() {
  // load resource for displaying entity contained_by and category
  $base_taxonomy_term_attr = variable_get('simple_feed_base_taxonomy_term_attributes');
  $base_taxonomy_term_attr_flip = array_flip($base_taxonomy_term_attr);
  $taxonomy_term_attr_dict = simple_feed_get_taxonomy_term_attr_dict();
  $field_map = field_info_field_map(); // in case the field map also contains fields for taxonomy vocabs
  $taxonomy_term_misc_attr_dict = simple_feed_get_taxonomy_term_misc_attr_dict();
  $filtered_taxonomy_term_attr = simple_feed_get_filtered_taxonomy_term_attr();

  // Build Options
  $term_attributes_options = array();
  foreach ($filtered_taxonomy_term_attr as $machine_name) {
    $term_attributes_options[$machine_name] = array();
    $term_attributes_options[$machine_name]['machine_name'] = $machine_name;

    if (isset($base_taxonomy_term_attr_flip[$machine_name])) {
      $term_attributes_options[$machine_name]['contained_by'] = t('All taxonomy terms');
      $term_attributes_options[$machine_name]['category'] = t('Base attributes');
    } else if (isset($field_map[$machine_name]['bundles']['taxonomy_term'])) {
      $term_attributes_options[$machine_name]['contained_by'] = t(implode(', ', $field_map[$machine_name]['bundles']['taxonomy_term']));
      $term_attributes_options[$machine_name]['category'] = t('Custom attributes');
    } else {
      $term_attributes_options[$machine_name]['contained_by'] = t(implode(', ', $taxonomy_term_misc_attr_dict[$machine_name]));
      $term_attributes_options[$machine_name]['category'] = t('Miscellaneous attributes');
    }
  }

  return $term_attributes_options;
}

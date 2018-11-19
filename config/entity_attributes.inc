<?php

/**
 * Entity Options (Node/Nodes and Taxonomy Term/Terms)
 */
function simple_feed_get_form_entity_attributes($profile_dict) {
  $form = array();

  // Set Tab Container
  $form['entity_attributes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Entity query settings'),
    '#group' => 'vertical_tabs',
  );

  // Set Node attributes shown in node/nodes query
  // ---------------------------------------------------------------------------
  // Build Table Headers
  $node_attributes_header = array(
    'machine_name' => array('data' => t('Machine Name')),
    'contained_by' => array('data' => t('Contained By')),
    'category' => array('data' => t('Category')),
  );

  // Set Form
  $form['entity_attributes']['node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node Attributes'),
    '#description' => t('Choose what node attributes you want to show in the node query json output.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['entity_attributes']['node']['node_attributes'] = array(
    '#type' => 'tableselect',
    '#header' => $node_attributes_header,
    '#options' => simple_feed_get_node_attr_options(),
    '#empty' => t('No options available.'),
    '#default_value' => simple_feed_array_to_form_selections(json_decode($profile_dict['node_attributes'], TRUE), simple_feed_get_filtered_node_attr()),
  );

  // Set Taxonomy term attributes shown in summary
  // ---------------------------------------------------------------------------
  // Build Table Headers
  $term_attributes_header = array(
    'machine_name' => array('data' => t('Machine Name')),
    'contained_by' => array('data' => t('Contained By')),
    'category' => array('data' => t('Category')),
  );

  // Set Form
  $form['entity_attributes']['taxonomy_term'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy Term Attributes'),
    '#description' => t('Choose what term attributes you want to show in the term query json output.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['entity_attributes']['taxonomy_term']['taxonomy_term_attributes'] = array(
    '#type' => 'tableselect',
    '#header' => $term_attributes_header,
    '#options' => simple_feed_get_taxonomy_term_attr_options(),
    '#empty' => t('No options available.'),
    '#default_value' => simple_feed_array_to_form_selections(json_decode($profile_dict['taxonomy_term_attributes'], TRUE), simple_feed_get_filtered_taxonomy_term_attr()),
  );

  return $form;

}

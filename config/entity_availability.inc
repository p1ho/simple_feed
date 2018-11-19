<?php

/**
 * Controls what nodes/taxonomy_terms this profile has access to.
 */
function simple_feed_get_form_entity_availability($profile_dict) {
  $form = array();

  // Set Tab Container
  $form['entity_availability'] = array(
    '#type' => 'fieldset',
    '#title' => t('Entity availability settings'),
    '#group' => 'vertical_tabs',
  );

  // Set whether unpublished nodes are shown
  $form['entity_availability']['allow_unpublished'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Show Unpublished Node'),
    '#options' => drupal_map_assoc(array('Enable')),
    '#default_value' => simple_feed_boolean_to_form_selections($profile_dict['allow_unpublished']==1, 'Enable'),
  );

  // Set types of Node available in this feed profile
  // ---------------------------------------------------------------------------
  $node_types = node_type_get_names();
  // Build Table Headers
  $node_types_header = array(
    'name' => array('data' => t('Human Readable Name')),
    'machine_name' => array('data' => t('Machine Name')),
  );
  // Build Options
  $node_types_options = array();
  foreach ($node_types as $machine_name => $name) {
    $node_types_options[$machine_name] = array(
      'name' => $name,
      'machine_name' => $machine_name,
    );
  }
  // Set Form
  $form['entity_availability']['node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node Types'),
    '#description' => t('Choose what node types are available.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['entity_availability']['node']['node_types_allowed'] = array(
    '#type' => 'tableselect',
    '#header' => $node_types_header,
    '#options' => $node_types_options,
    '#empty' => t('No options available.'),
    '#default_value' => simple_feed_array_to_form_selections(json_decode($profile_dict['node_types_allowed'], TRUE), array_keys($node_types)),
  );

  // Set types of Taxonomy Term available in this feed profile
  // ---------------------------------------------------------------------------
  $taxonomy_vocabs = simple_feed_taxonomy_vocabulary_get_names();
  // Build Table Headers
  $taxonomy_vocabs_header = array(
    'name' => array('data' => t('Human Readable Name')),
    'machine_name' => array('data' => t('Machine Name')),
  );
  // Build Options
  $taxonomy_vocabs_options = array();
  foreach ($taxonomy_vocabs as $machine_name => $name) {
    $taxonomy_vocabs_options[$machine_name] = array(
      'name' => $name,
      'machine_name' => $machine_name,
    );
  }
  // Set Form
  $form['entity_availability']['taxonomy_term'] = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy Vocabulary Types'),
    '#description' => t('Choose what vocabulary types are available.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['entity_availability']['taxonomy_term']['taxonomy_vocabs_allowed'] = array(
    '#type' => 'tableselect',
    '#header' => $taxonomy_vocabs_header,
    '#options' => $taxonomy_vocabs_options,
    '#empty' => t('No options available.'),
    '#default_value' => simple_feed_array_to_form_selections(json_decode($profile_dict['taxonomy_vocabs_allowed'], TRUE), array_keys($taxonomy_vocabs)),
  );

  return $form;

}

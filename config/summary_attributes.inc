<?php

/**
 * Summary Listing Options
 */
function simple_feed_get_form_summary_attributes($profile_dict) {
  $form = array();

  // Set Tab Container
  $form['summary'] = array(
    '#type' => 'fieldset',
    '#title' => t('Summary settings'),
    '#group' => 'vertical_tabs',
  );

  // Set Node attributes shown in summary
  // ---------------------------------------------------------------------------
  // Build Table Headers
  $node_attributes_header = array(
    'machine_name' => array('data' => t('Machine Name')),
    'contained_by' => array('data' => t('Contained By')),
    'category' => array('data' => t('Category')),
  );

  // Set Form
  $form['summary']['node'] = array(
    '#type' => 'fieldset',
    '#title' => t('Summary: Node Attributes'),
    '#description' => t('Choose what node attributes you want to show in the summary json output.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['summary']['node']['summary_node_attributes'] = array(
    '#type' => 'tableselect',
    '#header' => $node_attributes_header,
    '#options' => simple_feed_get_node_attr_options(),
    '#empty' => t('No options available.'),
    '#default_value' => simple_feed_array_to_form_selections(json_decode($profile_dict['summary_node_attributes'], TRUE), simple_feed_get_filtered_node_attr()),
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
  $form['summary']['taxonomy_term'] = array(
    '#type' => 'fieldset',
    '#title' => t('Summary: Taxonomy Term Attributes'),
    '#description' => t('Choose what term attributes you want to show in the summary json output.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['summary']['taxonomy_term']['summary_taxonomy_term_attributes'] = array(
    '#type' => 'tableselect',
    '#header' => $term_attributes_header,
    '#options' => simple_feed_get_taxonomy_term_attr_options(),
    '#empty' => t('No options available.'),
    '#default_value' => simple_feed_array_to_form_selections(json_decode($profile_dict['summary_taxonomy_term_attributes'], TRUE), simple_feed_get_filtered_taxonomy_term_attr()),
  );

  return $form;

}

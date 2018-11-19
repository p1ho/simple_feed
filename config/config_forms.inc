<?php

/**
 * Resources for building configuration pages
 */

/**
 * Profile Menu Form Page
 */
function simple_feed_get_profile_overview() {
  module_load_include('inc', 'simple_feed', 'resources/db_control');
  $all_profiles = simple_feed_get_all_profiles();

  $form = array();

  $header = array(
    'profile_name' => array('data' => t('Profile Name')),
    'description' => array('data' => t('Description')),
    'url' => array('data' => t('URL')),
    'operations' => array('data' => t('Operations')),
  );
  $rows = array();
  foreach ($all_profiles as $p) {
    $name = $p['name'];
    $host = $_SERVER['HTTP_HOST'];
    $path = "/simple_feed/$name";
    $rows[$name] = array(
      'profile_name' => $name,
      'description' => $p['description'],
      'url' => l($host.$path, $path),
      'operations' => implode('&nbsp;&nbsp;', array(
         l(t('Edit'), 'admin/config/development/simple_feed/profiles/list/'.$name),
         l(t('Delete'), 'admin/config/development/simple_feed/profiles/list/'.$name.'/delete'),
      )),
    );
  }

  $form['simple_feed_profile_statuses'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No profiles made'),
    '#default_value' => variable_get('simple_feed_profile_statuses'),
  );

  $form = system_settings_form($form);
  $form['#submit'][] = 'menu_rebuild';

  return $form;
}

/**
 * Default settings form page
 */
function simple_feed_get_default_settings_form() {
  module_load_include('inc', 'simple_feed');
  module_load_include('inc', 'simple_feed', 'config/helper_functions');
  module_load_include('inc', 'simple_feed', 'config/security');
  module_load_include('inc', 'simple_feed', 'config/entity_availability');
  module_load_include('inc', 'simple_feed', 'config/summary_attributes');
  module_load_include('inc', 'simple_feed', 'config/entity_attributes');
  module_load_include('inc', 'simple_feed', 'resources/default_settings');

  $settings = simple_feed_get_current_default_settings();

  $vert_tabs = array();
  $vert_tabs['vertical_tabs'] = array(
    '#type' => 'vertical_tabs',
    '#default_tab' => 'edit-security',
  );

  $form = array_merge(
    $vert_tabs,
    simple_feed_get_form_security($settings),
    simple_feed_get_form_entity_availability($settings),
    simple_feed_get_form_summary_attributes($settings),
    simple_feed_get_form_entity_attributes($settings)
  );

  $form = system_settings_form($form);
  $form['#submit'] = array(
    'simple_feed_security_validate',
    'simple_feed_default_settings_form_submit',
  );

  return $form;
}

/**
 * Default settings submission handler
 */
function simple_feed_default_settings_form_submit(&$form, &$form_state) {
  module_load_include('inc', 'simple_feed');

  // form sanitation and validation
  $response = $form_state['values'];
  $access_key = check_plain($response['access_key']);
  if (strlen($access_key) > 255) {
    $access_key = substr($access_key, 0, 255);
    drupal_set_message(t('Sanitized profile access-key was longer than 255 characters, it has been truncated.'), 'warning');
  }

  $response_processed = array(
    'access_key'                       => $access_key,
    'access_key_allow_in_url'          => (int)simple_feed_form_selections_to_boolean($response['access_key_allow_in_url']),
    'http_referer_restrict'            => (int)simple_feed_form_selections_to_boolean($response['http_referer_restrict']),
    'http_referers_allowed'            => json_encode($response['http_referers_allowed']), // processed in simple_feed_security_validate
    'ip_address_restrict'              => (int)simple_feed_form_selections_to_boolean($response['ip_address_restrict']),
    'ip_addresses_allowed'             => json_encode($response['ip_addresses_allowed']),  // processed in simple_feed_security_validate
    'allow_unpublished'                => (int)simple_feed_form_selections_to_boolean($response['allow_unpublished']),
    'node_types_allowed'               => json_encode(simple_feed_form_selections_to_array($response['node_types_allowed'])),
    'taxonomy_vocabs_allowed'          => json_encode(simple_feed_form_selections_to_array($response['taxonomy_vocabs_allowed'])),
    'summary_node_attributes'          => json_encode(simple_feed_form_selections_to_array($response['summary_node_attributes'])),
    'summary_taxonomy_term_attributes' => json_encode(simple_feed_form_selections_to_array($response['summary_taxonomy_term_attributes'])),
    'node_attributes'                  => json_encode(simple_feed_form_selections_to_array($response['node_attributes'])),
    'taxonomy_term_attributes'         => json_encode(simple_feed_form_selections_to_array($response['taxonomy_term_attributes'])),
  );

  foreach($response_processed as $key => $value) {
    variable_set($key, $value);
  }

  drupal_set_message(t('The configuration options have been saved.'), 'status');
}

/**
 * Add Profile form
 */
function simple_feed_get_add_profile_form() {
  $form = array();
  $form['profile_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Profile Name'),
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#required' => FALSE,
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Create'),
      '#submit' => array(
        'simple_feed_add_profile_form_submit',
      ),
    ),
    'cancel' => array(
        '#markup' => l(t('Cancel'), 'admin/config/development/simple_feed'),
    ),
  );

  return $form;
}

/**
 * Add Profile form submission handler
 */
function simple_feed_add_profile_form_submit(&$form, &$form_state) {
  module_load_include('inc', 'simple_feed', 'resources/db_control');

  // form sanitation and validation
  $name = check_plain($form_state['values']['profile_name']);
  $desc = check_plain($form_state['values']['description']);

  if (strlen($name) > 255) {
    $name = substr($name, 0, 255);
    drupal_set_message(t('Sanitized profile name was longer than 255 characters, it has been truncated.'), 'warning');
  }
  if (strlen($desc) > 255) {
    $desc = substr($desc, 0, 255);
    drupal_set_message(t('Sanitized profile description was longer than 255 characters, it has been truncated.'), 'warning');
  }

  // save setting to db
  simple_feed_add_profile($name, $desc);

  // redirect and set message
  $form_state['redirect'] = 'admin/config/development/simple_feed';
  drupal_set_message(t('Profile created successfully.'), 'status');

  // Rebuild Menu
  menu_rebuild();
}

/**
 * Profile configuration form
 */
function simple_feed_get_config_profile_form($profile) {
  module_load_include('inc', 'simple_feed');
  module_load_include('inc', 'simple_feed', 'resources/db_control');
  module_load_include('inc', 'simple_feed', 'config/helper_functions');
  module_load_include('inc', 'simple_feed', 'config/security');
  module_load_include('inc', 'simple_feed', 'config/entity_availability');
  module_load_include('inc', 'simple_feed', 'config/summary_attributes');
  module_load_include('inc', 'simple_feed', 'config/entity_attributes');

  $profile_dict = simple_feed_get_profile($profile);

  // For the submit handler to know what profile to update
  $profile_value = array(
    'profile_to_be_edited' => array(
      '#type' => 'value',
      '#value' => $profile,
    ),
  );

  $profile_name = array(
    'profile_name' => array(
      '#type' => 'item',
      '#title' => t('Profile Name'),
      '#markup' => $profile,
    )
  );

  $description = array(
    'description' => array(
      '#type' => 'textfield',
      '#title' => t('Description'),
      '#default_value' => $profile_dict['description'],
    )
  );

  $vert_tabs = array(
    'vertical_tabs' => array(
      '#type' => 'vertical_tabs',
      '#default_tab' => 'edit-security',
    ),
  );

  $actions = array(
    'actions' => array(
      '#type' => 'actions',
      'submit' => array(
        '#type' => 'submit',
        '#value' => t('Save'),
        '#submit' => array(
          'simple_feed_config_profile_form_submit',
        ),
      ),
      'submit-stay' => array(
        '#type' => 'submit',
        '#value' => t('Save and Continue Editing'),
        '#submit' => array(
          'simple_feed_config_profile_form_submit'
        ),
      ),
      'cancel' => array(
          '#markup' => l(t('Cancel'), 'admin/config/development/simple_feed'),
      ),
    ),
  );

  $form = array_merge(
    $profile_value,
    $profile_name,
    $description,
    $vert_tabs,
    simple_feed_get_form_security($profile_dict),
    simple_feed_get_form_entity_availability($profile_dict),
    simple_feed_get_form_summary_attributes($profile_dict),
    simple_feed_get_form_entity_attributes($profile_dict),
    $actions
  );

  return $form;
}

/**
 * Profile configuration form submission handler
 */
function simple_feed_config_profile_form_submit(&$form, &$form_state) {
  module_load_include('inc', 'simple_feed');
  module_load_include('inc', 'simple_feed', 'config/security'); // for onsubmit handler
  simple_feed_security_validate($form, $form_state);

  // form sanitation and validation
  $response = $form_state['values'];
  $desc = check_plain($response['description']);
  $access_key = check_plain($response['access_key']);
  if (strlen($desc) > 255) {
    $desc = substr($desc, 0, 255);
    drupal_set_message(t('Sanitized profile description was longer than 255 characters, it has been truncated.'), 'warning');
  }
  if (strlen($access_key) > 255) {
    $access_key = substr($access_key, 0, 255);
    drupal_set_message(t('Sanitized profile access-key was longer than 255 characters, it has been truncated.'), 'warning');
  }

  // save settings to database
  $name = $response['profile_to_be_edited'];
  $settings = array(
    'description'                      => $desc,
    'access_key'                       => $access_key,
    'access_key_allow_in_url'          => (int)simple_feed_form_selections_to_boolean($response['access_key_allow_in_url']),
    'http_referer_restrict'            => (int)simple_feed_form_selections_to_boolean($response['http_referer_restrict']),
    'http_referers_allowed'            => json_encode($response['http_referers_allowed']), // processed in simple_feed_security_validate
    'ip_address_restrict'              => (int)simple_feed_form_selections_to_boolean($response['ip_address_restrict']),
    'ip_addresses_allowed'             => json_encode($response['ip_addresses_allowed']),  // processed in simple_feed_security_validate
    'allow_unpublished'                => (int)simple_feed_form_selections_to_boolean($response['allow_unpublished']),
    'node_types_allowed'               => json_encode(simple_feed_form_selections_to_array($response['node_types_allowed'])),
    'taxonomy_vocabs_allowed'          => json_encode(simple_feed_form_selections_to_array($response['taxonomy_vocabs_allowed'])),
    'summary_node_attributes'          => json_encode(simple_feed_form_selections_to_array($response['summary_node_attributes'])),
    'summary_taxonomy_term_attributes' => json_encode(simple_feed_form_selections_to_array($response['summary_taxonomy_term_attributes'])),
    'node_attributes'                  => json_encode(simple_feed_form_selections_to_array($response['node_attributes'])),
    'taxonomy_term_attributes'         => json_encode(simple_feed_form_selections_to_array($response['taxonomy_term_attributes'])),
  );
  simple_feed_update_profile($name, $settings);

  // redirect and set message
  if ($form_state['triggering_element']['#parents'][0] === 'submit') {
    $form_state['redirect'] = 'admin/config/development/simple_feed';
  }
  drupal_set_message(t('Profile settings saved successfully.'), 'status');
}

/**
 * Profile delete confirmation form
 */

function simple_feed_get_delete_profile_form($profile) {
  $form = array();

  $form['profile_to_be_deleted'] = array(
    '#type' => 'value',
    '#value' => $profile,
  );

  $form['message'] = array(
    '#type' => 'item',
    '#title' => t('Are you sure?'),
    '#description' => t('You are deleting profile "@profile". This action cannot be reversed.', array('@profile' => $profile)),
  );

  $form['actions'] = array(
    '#type' => 'actions',
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array(
        'simple_feed_delete_profile_form_submit'
      ),
    ),
    'cancel' => array(
        '#markup' => l(t('Cancel'), 'admin/config/development/simple_feed'),
    ),
  );

  return $form;
}

/**
 * Profile delete confirmation form submission handler
 */
function simple_feed_delete_profile_form_submit(&$form, &$form_state) {
  module_load_include('inc', 'simple_feed', 'resources/db_control');

  // delete profile
  simple_feed_delete_profile($form_state['values']['profile_to_be_deleted']);

  // redirect and set message
  $form_state['redirect'] = 'admin/config/development/simple_feed';
  drupal_set_message(t('Profile deleted successfully.'), 'status');

  // Rebuild Menu
  menu_rebuild();
}

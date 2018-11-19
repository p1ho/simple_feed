<?php

/**
 * CRUD functions for manipulating simple feed profiles
 */

function simple_feed_add_profile($name, $desc) {
  // add new entry to database
  module_load_include('inc', 'simple_feed', 'resources/default_settings');
  $new_profile = array_merge(
    array(
      'name'        => check_plain($name),
      'description' => check_plain($desc),
    ),
    simple_feed_get_current_default_settings()
  );

  db_insert('simple_feed_profiles')->fields($new_profile)
                                   ->execute();
  variable_set('test', $new_profile);
  // append the new profile into profile_statuses
  $profile_statuses = variable_get('simple_feed_profile_statuses');
  $profile_statuses[$name] = 0;
  variable_set('simple_feed_profile_statuses', $profile_statuses);
}

function simple_feed_get_all_profiles() {
  module_load_include('inc', 'simple_feed');
  $result = db_select('simple_feed_profiles', 'pf')->fields('pf')
                                                   ->execute()
                                                   ->fetchAll();
  return simple_feed_to_array($result);
}

function simple_feed_get_profile($name) {
  return db_select('simple_feed_profiles', 'pf')->fields('pf')
                                                ->condition('name', $name, '=')
                                                ->execute()
                                                ->fetchAssoc();
}

function simple_feed_update_profile($name, $settings) {
  // throw error if there are settings that do not map to table field
  simple_feed_validate_profile_settings($settings);
  // update
  db_update('simple_feed_profiles')->condition('name', $name, '=')
                                   ->fields($settings)
                                   ->execute();
}

function simple_feed_delete_profile($name) {
  db_delete('simple_feed_profiles')->condition('name', $name, '=')
                                   ->execute();

  // remove profile from profile_statuses
  $profile_statuses = variable_get('simple_feed_profile_statuses');
  unset($profile_statuses[$name]);
  variable_set('simple_feed_profile_statuses', $profile_statuses);
}

function simple_feed_validate_profile_settings($settings) {
  $settings_keys = array_keys($settings);
  $table_field_lookup = array_flip(simple_feed_get_profiles_fields());
  foreach($settings_keys as $key) {
    if ($key === 'pfid') {
      throw new Exception('Please do not update profile id.');
    } else if ($key === 'name') {
      throw new Exception('Please do not update profile name.');
    } else {
      if (!isset($table_field_lookup[$key])) {
        throw new Exception("$key is not a valid field for Simple Feed profile settings.");
      }
    }
  }
}

/**
 * Helper function to get all fields from 'simple_feed_profile' table
 */
function simple_feed_get_profiles_fields() {
  $result = db_query('DESCRIBE {simple_feed_profiles}')->fetchAll();
  return array_map(function ($field) {return $field->Field;}, $result);
}

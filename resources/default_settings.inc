<?php

/**
 * Default Settings functions
 */

/**
 * Init default settings to system
 */
function simple_feed_init_default_settings() {
  $settings = simple_feed_get_original_default_settings();
  foreach ($settings as $key => $value) {
    variable_set($key, $value);
  }
}

/**
 * Remove default settings from system
 */
function simple_feed_del_default_settings() {
  $settings = simple_feed_get_original_default_settings();
  foreach($settings as $key => $value) {
    variable_del($key);
  }
}

/**
 * Get current default settings
 */
function simple_feed_get_current_default_settings() {
  $default_settings_original = simple_feed_get_original_default_settings();
  $default_settings_current = array();
  foreach($default_settings_original as $key => $value) {
    $default_settings_current[$key] = variable_get($key);
  }
  return $default_settings_current;
}

/**
 * Get true original default settings
 */
function simple_feed_get_original_default_settings() {
  return array(
    'access_key'              => 'please-change-this-key',
    'access_key_allow_in_url' => (int)FALSE,
    'http_referer_restrict'   => (int)FALSE,
    'http_referers_allowed'   => json_encode(array()),
    'ip_address_restrict'     => (int)FALSE,
    'ip_addresses_allowed'    => json_encode(array()),
    'allow_unpublished'       => (int)FALSE,
    'node_types_allowed'      => json_encode(array(
      'page',
      'article',
      'person',
      'newsletter',
    )),
    'taxonomy_vocabs_allowed' => json_encode(array(
      'topic',
    )),
    'summary_node_attributes' => json_encode(array(
      'nid',
      'title',
      'status',
      'type',
      'created',
      'changed',
      'url_original',
      'url_aliases',
    )),
    'summary_taxonomy_term_attributes' => json_encode(array(
      'tid',
      'vid',
      'name',
      'vocabulary_machine_name',
      'url_original',
      'url_aliases',
    )),
    'node_attributes' => json_encode(array(
      'nid',
      'title',
      'status',
      'type',
      'created',
      'changed',
      'body',
      'field_tags',
      'field_image',
      'field_authors',
      'field_external_url',
      'field_permanent',
      'field_topic',
      'field_newsletter',
      'field_archive',
      'field_page_layout',
      'field_youtube_video',
      'field_aw_gallery',
      'field_tweet_length_description',
      'newsletter_ts_2_reference',
      'metatags',
      'rdf_mapping',
      'url_original',
      'url_aliases',
    )),
    'taxonomy_term_attributes' => json_encode(array(
      'tid',
      'vid',
      'name',
      'description',
      'format',
      'weight',
      'vocabulary_machine_name',
      'url_original',
      'url_aliases',
    )),
  );
}

<?php

/**
 * Helper functions for simple_feed
 */

/**
 * Quick function return directory of setting
 */
function simple_feed_get_config_directory() {
  return 'admin/config/services/simple_feed';
}

 /**
  * Helper function to validate entity_types. Currently I only allow node
  * and taxonomy_term, but if the list expands, this function will be where
  * changes should be made.
  * @param  Array $entity_types [Array containing strings specifying entity type]
  * @return void                [If successfully returns, then we've validated]
  */
 function simple_feed_validate_entity_types($entity_types) {
   $accepted_types = array(
     "node",
     "taxonomy_term",
   );
   foreach ($entity_types as $type) {
     if (!in_array($type, $accepted_types)) {
       throw new Exception (t('Unknown entity type passed. You passed @type.', array('@type' => '"'.$type.'"')));
     }
   }
 }

/**
 * Helper function get all existing attributes under all node types specified
 * in the parameter.
 * @param  Array  $def_node_types [user set node types to search]
 * @return array [Array containing filtered node attributes]
 */
function simple_feed_get_filtered_node_attr($def_node_types = array()) {
  // load resources
  $base_node_attr = variable_get('simple_feed_base_node_attributes');
  $field_map = field_info_field_map();
  $custom_node_attr = array_keys(array_filter($field_map, function($x) {return isset($x['bundles']['node']);}));
  $node_attr_dict = array_keys(simple_feed_get_node_misc_attr_dict($def_node_types));

  return array_unique(array_merge($base_node_attr, $custom_node_attr, $node_attr_dict));
}

/**
 * Helper function get all existing attributes under every term types specified
 * in the parameter.
 * @param  Array  $def_term_types [user set term types to search]
 * @return array [Array containing filtered term attributes]
 */
function simple_feed_get_filtered_taxonomy_term_attr($def_taxonomy_vocabs = array()) {
  // load resources
  $base_taxonomy_term_attr = variable_get('simple_feed_base_taxonomy_term_attributes');
  $field_map = field_info_field_map(); // in case the field map also contains fields for taxonomy vocabs
  $custom_taxonomy_term_attr = array_keys(array_filter($field_map, function($x) {return isset($x['bundles']['taxonomy_term']);}));
  $taxonomy_term_attr_dict = array_keys(simple_feed_get_taxonomy_term_misc_attr_dict($def_taxonomy_vocabs));

  return array_unique(array_merge($base_taxonomy_term_attr, $custom_taxonomy_term_attr, $taxonomy_term_attr_dict));
}

/**
 * Helper function to get all attributes under a node type for every type.
 *
 * The approach is to load an existing node to get all attributes available,
 * this is more straight forward because it's tough to get attribute definition
 * without specifying a node id.
 *
 * Downside: if a content type is defined but no content has been made for it,
 * because we can't load any existing node, its unique definitions will be
 * overlooked.
 *
 * @param  Array  $def_node_types [user set node types to search]
 * @return array  [A dictionary with structure 'type' => [attr,...]]
 */
function simple_feed_get_node_attr_dict($def_node_types = array()) {
  // if def_node_types not specified, it'll return attr for every node types
  if (count($def_node_types) === 0) {
    $def_node_types = array_keys(node_type_get_names());
  }
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'node')
      ->propertyCondition('type', $def_node_types);
  $result = $query->execute();
  $nodes = (isset($result['node'])) ? $result['node'] : array();
  $node_type_dict = simple_feed_aggregate_to_dict(simple_feed_to_array($nodes), "type", "nid");

  $node_attr_dict = array();
  foreach ($def_node_types as $type) {
    if (isset($node_type_dict[$type])) {
      // 1. if a node id exists, load it
      $ref_id = (int)$node_type_dict[$type][0];
      $ref_node = simple_feed_entity_load_with_url('node', $ref_id);
      $node_attr_dict[$type] = array_keys(get_object_vars($ref_node));
    } else {
      // 2. if node doesn't exist, we can be confident no data for that content
      // type will be returned later, but we can't know for sure whether it's:
      //       a. node type doesn't exist
      //       b. node type exists but no content has been created
      // so we still need fail safe here that returns basic attributes.
      $node_attr_dict[$type] = variable_get('simple_feed_base_node_attributes');
    }
  }
  return $node_attr_dict;
}

/**
 * Helper function to create a dictionary with structure:
 * array( "misc attributes" => array("node type 1", "node type 2", ...))
 *
 * @param  Array  $def_node_types [user set node types to search]
 * @return array
 */
function simple_feed_get_node_misc_attr_dict($def_node_types = array()) {
  // load resources
  $base_node_attr = variable_get('simple_feed_base_node_attributes');
  $base_node_attr_flip = array_flip($base_node_attr);
  $field_map = field_info_field_map();
  $node_attr_dict = simple_feed_get_node_attr_dict($def_node_types);

  $node_misc_attr_dict = array();
  foreach (array_keys($node_attr_dict) as $node_type) {
    foreach ($node_attr_dict[$node_type] as $attr) {
      if (!isset($base_node_attr_flip[$attr]) && !isset($field_map[$attr])) {
        if (!isset($node_misc_attr_dict[$attr])) {
          $node_misc_attr_dict[$attr] = array($node_type);
        } else {
          $node_misc_attr_dict[$attr][] = $node_type;
        }
      }
    }
  }
  return $node_misc_attr_dict;
}

/**
 * Helper function to get all attributes under a taxonomy vocab for every vocab.
 *
 * The approach is to load an existing term under a vocab to get all attributes
 * available for that vocab, this is more straight forward because it's tough to
 * get attribute definition without specifying a vocab id.
 *
 * Downside: if a vocab type is defined but no term has been made for it,
 * because we can't load any existing term, its unique definitions will be
 * overlooked.
 *
 * @param  Array  $vocab_types [user set taxonomy vocabs to search]
 * @return array               [A dictionary with structure 'vocab' => [attr,...]]
 */
function simple_feed_get_taxonomy_term_attr_dict($def_taxonomy_vocabs = array()) {
  // if def_taxonomy_vocabs not specified, it'll return attributes for all vocab types
  $taxonomy_vocab_dict = taxonomy_vocabulary_get_names();
  if (count($def_taxonomy_vocabs) === 0) {
    $def_taxonomy_vocabs = array_keys($taxonomy_vocab_dict);
  }
  $def_taxonomy_vocab_dict = array();
  foreach ($def_taxonomy_vocabs as $vocab) {
    $def_taxonomy_vocab_dict[$vocab] = $taxonomy_vocab_dict[$vocab]->vid;
  }
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', 'taxonomy_term')
      ->propertyCondition('vid', array_values($def_taxonomy_vocab_dict));
  $result = $query->execute();
  $terms = (isset($result['taxonomy_term'])) ? $result['taxonomy_term'] : array();
  $term_type_dict = simple_feed_aggregate_to_dict(simple_feed_to_array($terms), "vid", "tid");

  $term_attr_dict = array();
  foreach ($def_taxonomy_vocab_dict as $vocab=>$vid) {
    if (isset($term_type_dict[$vid])) {
      // 1. if a vocab id exists, load it
      $ref_id = (int)$term_type_dict[$vid][0];
      $ref_term = simple_feed_entity_load_with_url('taxonomy_term', $ref_id);
      $term_attr_dict[$vocab] = array_keys(get_object_vars($ref_term));
    } else {
      // 2. if term doesn't exist, we can be confident no data for that term
      // type will be returned later, but we can't know for sure whether it's:
      //       a. taxonomy vocab doesn't exist
      //       b. taxonomy vocab exists but no term has been created
      // so we still need fail safe here that returns basic attributes.
      $term_attr_dict[$vocab] = variable_get('simple_feed_base_taxonomy_term_attributes');
    }
  }
  return $term_attr_dict;
}

/**
 * Helper function to create a dictionary with structure:
 * array( "misc attributes" => array("vocab 1", "vocab 2", ...))
 *
 * @param  Array  $vocab_types [user set taxonomy vocabs to search]
 * @return array
 */
function simple_feed_get_taxonomy_term_misc_attr_dict($def_taxonomy_vocabs = array()) {
  // load resources
  $base_taxonomy_term_attr = variable_get('simple_feed_base_taxonomy_term_attributes');
  $base_taxonomy_term_attr_flip = array_flip($base_taxonomy_term_attr);
  $field_map = field_info_field_map(); // in case the field map also contains fields for taxonomy vocabs
  $taxonomy_term_attr_dict = simple_feed_get_taxonomy_term_attr_dict($def_taxonomy_vocabs);

  $taxonomy_term_misc_attr_dict = array();
  foreach (array_keys($taxonomy_term_attr_dict) as $vocab) {
    foreach ($taxonomy_term_attr_dict[$vocab] as $attr) {
      if (!isset($base_taxonomy_term_attr_flip[$attr]) && !isset($field_map[$attr])) {
        if (!isset($taxonomy_term_misc_attr_dict[$attr])) {
          $taxonomy_term_misc_attr_dict[$attr] = array($vocab);
        } else {
          $taxonomy_term_misc_attr_dict[$attr][] = $vocab;
        }
      }
    }
  }
  return $taxonomy_term_misc_attr_dict;
}

/**
 * The original taxonomy_vocabulary_get_names(), instead of returning an
 * associative array with the format array("machine_name" => "name"), actually
 * returns an array of objects.
 * For consistency, this was implemented so the returned format is the same as
 * node_type_get_names().
 * @return array [array("machine_name" => "name")]
 */
function simple_feed_taxonomy_vocabulary_get_names() {
  return array_map(function($x) {return $x->name;}, taxonomy_vocabulary_get_names());
}

/**
 * Helper function to convert selected taxonomy vocabularies to an array of
 * taxonomy_vocab_id. If a selection is not available on a site, it is omitted.
 * @return array
 */
function simple_feed_taxonomy_vocabulary_get_selected_id($vocabs_allowed) {
  $taxonomy_vocab_dict = simple_feed_to_array(taxonomy_vocabulary_get_names());
  $taxonomy_vocabs_id = array();
  foreach ($vocabs_allowed as $vocab) {
    if (isset($taxonomy_vocab_dict[$vocab])) {
      $taxonomy_vocabs_id[] = $taxonomy_vocab_dict[$vocab]['vid'];
    }
  }
  return $taxonomy_vocabs_id;
}

/**
 * Helper function to get all url aliases
 * @param  Array $entity_types [Contains array of strings]
 * @return array [array with structure 'url_original' => ['url_alias','url_alias','url_alias']]
 */
function simple_feed_get_all_url_aliases($entity_types) {
  simple_feed_validate_entity_types($entity_types);
  $entity_types_count = count($entity_types);
  for ($i = 0; $i < $entity_types_count; $i++) {
    if ($entity_types[$i] === "taxonomy_term") {$entity_types[$i] = "taxonomy";}
  }
  $sql = 'SELECT source, alias FROM {url_alias} ';
  $sql_params = array();
  for ($i = 0; $i < $entity_types_count; $i++) {
    if ($i === 0) {
      $sql .= 'WHERE source LIKE :entity_type0 ';
    } else {
      $sql .= 'OR source LIKE :entity_type' . $i. ' ';
    }
    $sql_params[":entity_type$i"] = $entity_types[$i]."/%";
  }
  $query = db_query($sql, $sql_params);
  $result = $query->fetchAll();
  // reorganize into associative array
  return simple_feed_aggregate_to_dict(simple_feed_to_array($result), "source", "alias");
}

/**
 * Helper function to get url aliases for a single entity
 *
 * Note: I could potentially just turn this into a wrapper function for
 * simple_feed_get_all_url_aliases(), but because this approach changes at the
 * SQL query level, it should be more efficient, albeit slightly redundant.
 * @param  String         $entity_type
 * @param  String|Integer $id
 * @return array [array with structure 'url_original' => ['url_alias','url_alias','url_alias']]
 */
function simple_feed_get_url_aliases($entity_type, $id) {
  simple_feed_validate_entity_types(array($entity_type));
  if ($entity_type === "taxonomy_term") {$entity_type = "taxonomy/term";}
  $sql = 'SELECT source, alias FROM {url_alias} '
       . 'WHERE source = :url_original';
  $sql_params = array(":url_original" => $entity_type . '/' . $id);
  $query = db_query($sql, $sql_params);
  $result = $query->fetchAll();
  // reorganize into associative array
  return simple_feed_aggregate_to_dict(simple_feed_to_array($result), "source", "alias");
}

/**
 * Helper function to get Entity Data of the same entity_type and the attributes
 * we set in config will limit what attributes get returned
 * @param  Array  $loaded_entities [Array containing entity objects]
 * @param  Array  $attr_types      [Array containing attributes to use]
 * @return array                   [packaged data for the entity type entered]
 */
function simple_feed_trim_entity_attr($loaded_entities, $attr_types) {
  $attr_types_flip = array_flip($attr_types);
  $entity_data = array();
  foreach ($loaded_entities as $entity) {
    $entity_arr = (array)$entity;
    $entity_keys = array_keys($entity_arr);
    foreach ($entity_keys as $key) {
      if (!isset($attr_types_flip[$key])) {
        unset($entity_arr[$key]);
      }
    }
    // this solves the problem of json encoding empty associative array as a list instead of an object in JSON
    $entity_final = count($entity_arr) !== 0 ? $entity_arr : new stdClass;
    $entity_data[] = $entity_final;
  }
  return $entity_data;
}

/**
 * Wrapper function around entity_load() to add url info to objects. But I've
 * put in a mechanism to stop it from returning entity types we do not want to
 * output such as users.
 * Note:
 * - The parameter conditions is deprecated, so not included here
 * @param  String $entity_type [The entity type to load]
 * @param  Array  $ids         [An array of entity IDs]
 * @param  Bool   $reset       [Whether to reset internal cache]
 * @return array               [An array of entity objects indexed by their ids.
 *                              When no results are found, an empty array is
 *                              returned.]
 */
function simple_feed_entity_load_multiple_with_url($entity_type, $ids, $reset = FALSE) {
  simple_feed_validate_entity_types(array($entity_type));
  if ($entity_type === "node") {
    $url_base = "node/";
    $entity_id = "nid";
  } else if ($entity_type === "taxonomy_term") {
    $url_base = "taxonomy/term/";
    $entity_id = "tid";
  } else {
    // theoretically if simple_feed_validate_entity_types() runs without error this block
    // should never be entered.
    return array();
  }
  $entity_obj_arr = entity_load($entity_type, $ids, $reset);
  $url_aliases = simple_feed_get_all_url_aliases(array($entity_type));
  // the built-in entity_load() functions will return array of only ids found
  // (with no empty object or NULL put in place for those not found),
  // so it's not necessary to check individual items in $entity_obj_arr for
  // NULL or FALSE.
  foreach ($entity_obj_arr as $entity_obj) {
    $url_original = $url_base . $entity_obj->$entity_id;
    $entity_obj->url_original = $url_original;
    $entity_obj->url_aliases = (isset($url_aliases[$url_original])) ? $url_aliases[$url_original] : array();
  }
  return $entity_obj_arr;
}

/**
 * Wrapper around simple_feed_entity_load_multiple_with_url(), basically making it so it
 * is consistent with node_load() and node_load_multiple() core functions.
 * @param  String $entity_type [The entity type to load]
 * @param  String|Integer $id  [entity id]
 * @param  Bool   $reset       [Whether to reset internal cache]
 * @return object              [Entity Object]
 */
function simple_feed_entity_load_with_url($entity_type, $id, $reset = FALSE) {
  $result = simple_feed_entity_load_multiple_with_url($entity_type, array($id), $reset);
  if (count($result) !== 0) {
    return $result[$id];
  } else {
    return FALSE;
  }
}

/**
 * simple_feed_aggregate_to_dict function. Takes in an array containing items with
 * consistent fields. Will create a dictionary with keys taken from one field
 * from the source_array, and its values another field from rows with the same
 * value for the field specified as key. If key or value specified are not
 * found in a row, the row is skipped.
 *
 * For example:
 * consider a $test_array:
 * [
 *  [
 *    'a' => 'foo'
 *    'b' => 1
 *  ]
 *  [
 *    'a' => 'foo'
 *    'b' => 3
 *  ]
 *  [
 *    'a' => 'bar'
 *    'b' => 3
 *  ]
 * ]
 *
 * calling simple_feed_aggregate_to_dict($test_array, 'a', 'b') will return
 * [
 *  'foo' => [1,3]
 *  'bar' => [3]
 * ]
 *
 * calling simple_feed_aggregate_to_dict_($test_array, 'b', 'a') will return
 * [
 *  1 => ['foo']
 *  3 => ['foo', 'bar']
 * ]
 *
 *
 * @param  Array          $src_array
 * @param  String|Integer $key       [name of field to use as key]
 * @param  String|Integer $value     [name of field to aggregate under key]
 * @return array          [aggregated to a dictionary]
 */
function simple_feed_aggregate_to_dict($src_array, $key, $value) {
  if (count($src_array) === 0) {return array();}
  $return_array = array();
  foreach($src_array as $row) {
    if (!isset($row[$key]) || !isset($row[$value])) {
      continue;
    }
    if (!isset($return_array[$row[$key]])) {
      $return_array[$row[$key]] = array($row[$value]);
    } else {
      $return_array[$row[$key]][] = $row[$value];
    }
  }
  return $return_array;
}

/**
 * simple_feed_to_array.
 * @param  Array|Object $input [anything convertible to array]
 * @return array
 */
function simple_feed_to_array($input) {
  return json_decode(json_encode($input), true);
}

/**
 * Helper function to convert settings (in array form) to drupal format
 * (associative array with all options as keys, and values to indicate selected or not)
 * If a selection is not in the $choices array, it will disregard it.
 *
 * Assumption: all choices are unique. If you pass in an array with duplicate
 * values, because it gets converted to an associative array, one of those values
 * will be lost.
 *
 * @param  Array  $selections [Array containing list of selections]
 * @param  Array  $choices    [Array containing list of possible options]
 * @return Array              [Associative array with the format
 *                            Array(
 *                              "option1" => 0, <--Drupal's format for not selected
 *                              "option2" => "option2",
 *                            )]
 */
function simple_feed_array_to_form_selections($selections, $choices) {
 $drupal_form = drupal_map_assoc($choices);
 $choices_flip = array_flip($choices);
 array_walk($drupal_form, function(&$a, $b){$a = 0;});
 foreach($selections as $sel) {
   if (isset($choices_flip[$sel])) {
     $drupal_form[$sel] = $sel;
   }
 }
 return $drupal_form;
}

/**
 * Helper function to convert settings in drupal format (associative array with
 * all options as keys, and values to indicate selection) back to a simple array
 * of only selected value.
 * @param  Array  $selections [Associative array with the format
 *                            Array(
 *                              "option1" => 0, <--Drupal's format for not selected
 *                              "option2" => "option2",
 *                            )]
 * @return Array              [Array containing list of selections]
 */
function simple_feed_form_selections_to_array($selections) {
 $selections_array = array_values($selections);
 foreach (array_keys($selections_array, '0') as $key) {
   unset($selections_array[$key]);
 }
 return array_values($selections_array); // reorder array
}

/**
 * Helper function to quickly generate array for checkboxes type that only
 * has a single option.
 * @param  Boolean $checked [if choice is selected]
 * @param  String  $choice  [string of the choice]
 * @return array            [Associative array]
 */
function simple_feed_boolean_to_form_selections($checked, $choice) {
  return $checked ? array($choice=>$choice) : array($choice => 0);
}

/**
 * Helper function to quickly convert response array back to boolean for
 * checkboxes type that only has a single option.
 * @param  Array   $selections [Associative array of response]
 * @return boolean             [Whether choice was selected]
 */
function simple_feed_form_selections_to_boolean($selections) {
  return array_values($selections)[0] !== 0;
}

/**
 * Parse id string passed in the url
 * @param  String $s [comma delimited string of ids]
 * @return array     [parsed ids]
 */
function simple_feed_parse_ids_str($s) {
  $ids = explode(",", $s);
  $ids_size = count($ids);
  for ($i = 0; $i < $ids_size; $i++) {
    if (trim($ids[$i]) === "") {
      unset($ids[$i]);
    }
  }
  return array_values($ids);
}

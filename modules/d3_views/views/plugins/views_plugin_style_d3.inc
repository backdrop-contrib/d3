<?php

/**
 * @file
 * Contains the d3 style plugin.
 */

/**
 * Style plugin to render a d3 visualization
 *
 * @ingroup views_style_plugins
 */
class views_plugin_style_d3 extends views_plugin_style {

  protected $libraries = array();

  /**
   * Set default options
   */
  function option_definition() {
    $options = parent::option_definition();

    $options['library'] = array('default' => FALSE);
    $options['columns'] = array('default' => array());

    return $options;
  }

  /**
   * Render the given style.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    // We temporarily disable grouping on all libraries. 
    // @TODO In future releases there should be an option to group
    // if the visualization's data requires it.
    $form['grouping']['#access'] = FALSE;

    $form['library'] = array(
      '#title' => 'Library',
      '#type' => 'select',
      '#options' => $this->get_libraries(),
      '#default_value' => $this->options['library'],
    );

    $handlers = $this->display->handler->get_handlers('field');
    if (empty($handlers)) {
      $form['error_markup'] = array(
        '#markup' => '<div class="error messages">' . t('You need at least one field before you can configure your table settings') . '</div>',
      );
      return;
    }

    // Note: views UI registers this theme handler on our behalf. Your module
    // will have to register your theme handlers if you do stuff like this.
    $form['#theme'] = 'views_ui_style_d3_plugin_table';

    // Create an array of allowed columns from the data we know:
    $field_names = $this->display->handler->get_field_labels();


    foreach ($field_names as $field => $display) {
      $safe = str_replace(array('][', '_', ' '), '-', $field);
      // the $id of the column for dependency checking.
      $id = 'edit-style-options-columns-' . $safe;

      // markup for the field name
      $form['columns'][$field]['name'] = array(
        '#markup' => $field_names[$field],
      );

      $form['columns'][$field]['type'] = array(
        '#type' => 'select',
        '#options' => array('_none' => 'None', 'column' => 'Column', 'row' => 'Row label'),
        '#default_value' => isset($this->options['columns'][$field]['type']) ? $this->options['columns'][$field]['type'] : '',
      );

    }

  }
 
  /**
   * Normalize a list of columns to use for the visualization
   * as well as a list of row labels.
   * 
   * In the example of a column chart, the column will be the
   * name in the legend, and represent the meaning of the value of
   * each bar. The row label will represent the X-Axis labels.
   *
   * @return object
   *    ->columns = array()
   *      An array of column keys
   *    ->labels = array()
   *      An array of label keys
   */
  function columns() {
    static $columns;

    if ($columns) {
      return $columns;
    }

    // Allows chaining. Ex: $this->columns()->labels;
    $columns = new stdClass();
    $columns->columns = array();
    $columns->labels = array();

    // We want to cross reference with the fields associated with the view
    // and maintain that order as well.
    $fields = $this->display->handler->get_handlers('field');
    $field_labels = $this->display->handler->get_field_labels();
    foreach ($fields as $field) {
      // Get the machine name for the field.
      $field = $field->field;

      // If this isn't a valid column for whatever reason, do not use it.
      if (!isset($this->options['columns'][$field]['type'])) {
        continue;
      }

      $settings = $this->options['columns'][$field];
      // We only allow for these two scenarios in case the user
      // has chose another setting for this particular field on the options
      // form.
      if ($settings['type'] == 'column') {
        $columns->columns[$field] = $field_labels[$field]; 
      }
      if ($settings['type'] == 'row') {
        $columns->labels[$field] = $field_labels[$field]; 
      }

    }

    return $columns;
  }

  /**
   * Render the display in this style.
   */
  function render() {
    if ($this->uses_row_plugin() && empty($this->row_plugin)) {
      debug('views_plugin_style_default: Missing row plugin');
      return;
    }

    // Group the rows according to the grouping instructions, if specified.
    $sets = $this->render_grouping(
      $this->view->result,
      $this->options['grouping'],
      TRUE
    );

    // Since grouping is disabled right now we can just fetch the 
    // rows directly
    $set = array_shift($sets);
    $rows = $this->preprocess($set['rows']);

    return $this->draw($rows);
  }

  /**
  * Get the raw field value.
  *
  * @param $index
  *   The index count of the row.
  * @param $field
  *    The id of the field.
  */
  function get_field_value($index, $field) {
    $this->view->row_index = $index;
    $value = $this->view->field[$field]->get_value($this->view->result[$index]);
    unset($this->view->row_index);
    return $value;
  }

  function preprocess($rows) {
    $data = array();
    // an array for the grouped fields displays to show on the visualization 
    // for each of the fields associated with this view
    foreach ($rows as $index => $row) {
      // Get the display name for the row label fields that were
      // chosen on the options page.
      $labels = array();
      foreach ($this->columns()->labels as $label => $name) {
        $labels[] = $this->get_field_label($index, $label);
      }

      // Create a new array for this data row, and start with the labels
      // joined by a ' ' if necessary.
      $data_row = array(join(' ', $labels));
      // Now add each column to the rest of the data row.
      foreach ($this->columns()->columns as $column => $name) {
        $value = $this->get_field_value($index, $column);
        $data_row[$column] = isset($data[$column]) ? ($data[$column] + $value[0]['value']) : $value[0]['value'];
      }
      $data[] = array_values($data_row);
    }

    return $data; 
  }

  function draw($rows) {
    $columns = $this->columns()->columns;
    $field_labels = $this->display->handler->get_field_labels();
      
    $legend = array();
    foreach ($columns as $column => $settings) {
      $legend[] = $field_labels[$column];
    }

    $this->get_libraries();
    $type = $this->libraries[$this->options['library']]['js callback'];
    $chart = array(
      'id' => 'visualization',
      'type' => $type,
      'legend' => $legend,
      'rows' => $rows,
    );

    return d3_draw($chart);
  }

  function get_field_label($index, $field) {
    $group_content = '';

    if (isset($this->view->field[$field])) {
      $group_content = $this->get_field($index, $field);
      if ($this->view->field[$field]->options['label']) {
        $group_content = $this->view->field[$field]->options['label'] . ': ' . $group_content;
      }
    }

    return $group_content;
  }

  /**
   * Group records as needed for rendering.
   *
   * @param $records
   *   An array of records from the view to group.
   * @param $groupings
   *   An array of grouping instructions on which fields to group. If empty, the
   *   result set will be given a single group with an empty string as a label.
   * @param $group_rendered
   *   Boolean value whether to use the rendered or the raw field value for
   *   grouping. If set to NULL the return is structured as before
   *   Views 7.x-3.0-rc2. After Views 7.x-3.0 this boolean is only used if
   *   $groupings is an old-style string or if the rendered option is missing
   *   for a grouping instruction.
   * @return
   *   The grouped record set.
   *   A nested set structure is generated if multiple grouping fields are used.
   *
   *   @code
   *   array(
   *     'grouping_field_1:grouping_1' => array(
   *       'group' => 'grouping_field_1:content_1',
   *       'rows' => array(
   *         'grouping_field_2:grouping_a' => array(
   *           'group' => 'grouping_field_2:content_a',
   *           'rows' => array(
   *             $row_index_1 => $row_1,
   *             $row_index_2 => $row_2,
   *             // ...
   *           )
   *         ),
   *       ),
   *     ),
   *     'grouping_field_1:grouping_2' => array(
   *       // ...
   *     ),
   *   )
   *   @endcode
   */
  function render_grouping($records, $groupings = array(), $group_rendered = NULL) {
    // This is for backward compability, when $groupings was a string containing
    // the ID of a single field.
    if (is_string($groupings)) {
      $rendered = $group_rendered === NULL ? TRUE : $group_rendered;
      $groupings = array(array('field' => $groupings, 'rendered' => $rendered));
    }

    // Make sure fields are rendered
    $this->render_fields($this->view->result);
    $sets = array();
    if ($groupings) {
      foreach ($records as $index => $row) {
        // Iterate through configured grouping fields to determine the
        // hierarchically positioned set where the current row belongs to.
        // While iterating, parent groups, that do not exist yet, are added.
        $set = &$sets;
        foreach ($groupings as $info) {
          $field = $info['field'];
          $rendered = isset($info['rendered']) ? $info['rendered'] : $group_rendered;
          $rendered_strip = isset($info['rendered_strip']) ? $info['rendered_strip'] : FALSE;
          $grouping = '';
          $group_content = '';
          // Group on the rendered version of the field, not the raw.  That way,
          // we can control any special formatting of the grouping field through
          // the admin or theme layer or anywhere else we'd like.
          if (isset($this->view->field[$field])) {
            $group_content = $this->get_field($index, $field);
            if ($this->view->field[$field]->options['label']) {
              $group_content = $this->view->field[$field]->options['label'] . ': ' . $group_content;
            }
            if ($rendered) {
              $grouping = $group_content;
              if ($rendered_strip) {
                $group_content = $grouping = strip_tags(htmlspecialchars_decode($group_content));
              }
            }
            else {
              $grouping = $this->get_field_value($index, $field);
              // Not all field handlers return a scalar value,
              // e.g. views_handler_field_field.
              if (!is_scalar($grouping)) {
                $grouping = md5(serialize($grouping));
              }
            }
          }

          // Create the group if it does not exist yet.
          if (empty($set[$grouping])) {
            $set[$grouping]['group'] = $group_content;
            $set[$grouping]['rows'] = array();
          }

          // Move the set reference into the row set of the group we just determined.
          $set = &$set[$grouping]['rows'];
        }
        // Add the row to the hierarchically positioned row set we just determined.
        $set[$index] = $row;
      }
    }
    else {
      // Create a single group with an empty grouping field.
      $sets[''] = array(
        'group' => '',
        'rows' => $records,
      );
    }

    // If this parameter isn't explicitely set modify the output to be fully
    // backward compatible to code before Views 7.x-3.0-rc2.
    // @TODO Remove this as soon as possible e.g. October 2020
    if ($group_rendered === NULL) {
      $old_style_sets = array();
      foreach ($sets as $group) {
        $old_style_sets[$group['group']] = $group['rows'];
      }
      $sets = $old_style_sets;
    }

    return $sets;
  }

  function get_libraries() {
    static $views_libraries;

    if ($views_libraries) {
      return $views_libraries;
    }

    $views_api_version = views_api_version();

    $libraries = libraries_info();
    $views_libraries = array();

    foreach ($libraries as $library) {
      // We can match compatibility based on the views API version.
      if (isset($library['views']) && $library['views'] <= $views_api_version) {
        $library_name = $library['machine name'];
        $library['js callback'] = str_replace('d3.', '', $library_name);
        $views_libraries[$library_name] = $library['name'];
        $this->libraries[$library_name] = $library;
      }
    }

    return $views_libraries;
  }
}
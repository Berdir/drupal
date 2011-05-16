<?php

/**
 * @file
 * Contains classes for fine grained cache access.
 */

class ArrayAccessCache implements ArrayAccess {

  protected $cache_key;
  protected $isLoaded = FALSE;

  protected $data_keys = array();

  protected $data = array();

  protected function __construct($key, array $data = array()) {
    $this->cache_key = $key;
    $this->data = $data;
  }

  public static function create($key) {
    return new ArrayAccessCache($key);
  }

  public function load() {
    if ($cached = cache_get($this->cache_key)) {
      $this->data_keys = $cached->data;
      $this->isLoaded = TRUE;
      return TRUE;
    }
  }

  public function isLoaded() {
    return $this->isLoaded;
  }

  public function merge($new_data) {
    $this->data = array_merge($this->data, $new_data);
  }

  public function saveToCache() {
    cache_set_multiple($this->cache_key . ':array:', $this->data);
    cache_set($this->cache_key, array_keys($this->data));
  }

  public function offsetExists($offset) {
    if (isset($this->data[$offset])) {
      return TRUE;
    }

    if (in_array($offset, $this->data_keys) && $cached = cache_get($this->cache_key . ':array:' . $offset)) {
      $this->data[$offset] = $cached->data;
      return TRUE;
    }
  }

  public function offsetGet($offset) {
    // Return an ArrayObject so that indirect referencing works.
    return new ArrayObject($this->data[$offset]);
  }

  public function offsetSet($offset, $value) {
    return $this->data[$offset] = $value;
  }

  public function offsetUnset($offset) {
    unset($this->data[$offset]);
  }
}
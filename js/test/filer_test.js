var vows = require('vows'),
  assert = require('assert'),
  Filer  = require('../lib/models/filer'),
  dbSupport = require('./support/db');

var graph   = dbSupport.graph;

vows.describe('Filer').addBatch({
  'persists': {
    topic: function() { return new Filer(graph, {}); },
    'it allows variables to be assigned and saved': function(topic) {
      topic.id = 3;
      topic.name = 'PAC Filer Man';
      assert.equal(topic.id, 3);
      assert.equal(topic.name, 'PAC Filer Man');

      topic.then(function(filer) {
        filer.save().then( function(node) {
          var retrieved_topic = graph.node;
          assert.equal(retrieved_topic.id, 3);
          assert.equal(retrieved_topic.name, 'PAC Filer Man');
        });
      });
    },
    teardown: function(topic) {
      dbSupport.clear(graph);
    }
  }
}).export(module);

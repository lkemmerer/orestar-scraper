var vows      = require('vows'),
    assert    = require('assert'),
    dbSupport = require('./support/db'),
    Importer  = require('../lib/orestar_importer');

var graph   = dbSupport.graph;

vows.describe('Importer').addBatch({
  'knows what is what': {
    topic: function() { return new Importer('./../fixtures/test_file.xls', graph) },
    'can read an xls': function(topic) {
      topic.rows(function(err, results) { 
        console.log([err, results]);
        assert.equal(results, [
          { id: '15', name: 'PAC Filer Man' },
          { id: '42', name: 'Anna Lisa' }
        ]);
      });
    },
    teardown: function() {
      dbSupport.clear(graph);
    }
  }
}).export(module);

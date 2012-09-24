var vows      = require('vows'),
    assert    = require('assert'),
    dbSupport = require('./support/db'),
    Importer  = require('../lib/orestar_importer');

var graph   = dbSupport.graph;

vows.describe('Importer').addBatch({
  'knows what is what': {
    topic: function() { return new OrestarImporter(testFile, graph) },
    'can read a csv': function(topic) {
      topic.rows().should == [
        { id: '15', name: 'PAC Filer Man' },
        { id: '42', name: 'Anna Lisa' }
      ]
    },
    teardown: function() {
      dbSupport.clear(graph);
    }
  }
}).export(module);

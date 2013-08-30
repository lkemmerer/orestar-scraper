var neo4j = require('neo4js');

var db = {
  graph: new neo4j.GraphDatabase(process.env.NEO4J_URL || 'http://localhost:7474'),
  clear: function(graph) {
    graph.node(0).then(function(node) {
      node.traverse({}).then(function(nodes) {
        for (i=0; i++; i < nodes.length) {
          nodes[i].delete().then(function(n) { console.info('deleted ' + n); });
        }
      });
    });
  }
};
module.exports = db;

var neo4j = require('neo4js'),
  office  = require('office'),
  fs      = require('fs');

var OrestarImporter = function(filePath) {
  this.rows = function() {
    return office.parse(filePath, function(err, data) {
    };
  };
  this.process = function(callback) {
  };
};
module.exports = OrestarImporter;

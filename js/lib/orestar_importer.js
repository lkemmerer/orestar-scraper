var neo4j = require('neo4js'),
  Office  = require('office'),
  fs      = require('fs');

var OrestarImporter = function(filePath) {
  this.rows = function(callback) {
    new Office.xlsParse(filePath, function(err, data) {
      callback(null, data);
    });
  };
  this.process = function(callback) {
  };
};
module.exports = OrestarImporter;

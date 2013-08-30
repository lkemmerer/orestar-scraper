var neo4j = require('neo4js'),
  util = require('util');

var Filer = function Filer(db, data, callback) {
  var _db = db;
  var _node = _db.node(data, function(err, result) { console.log(err || result); _node = result;});

  this.save = function(callback) {
    return _node.save(callback);
  }

  this.then = function(callback) {
    return _node.then(callback); 
  }
};

function proxyProperty(prop) {
  Object.defineProperty(Filer.prototype, prop, {
    get: function() {
      return this._node.getProperty(prop);
    },
    set: function(value) {
      this._node.setProperty(prop, value);
    }
  });
}

proxyProperty('id');
proxyProperty('name');

util.inherits(Filer, neo4j.models.PropertyContainer);

module.exports = Filer;

var request = require('request'),
    fs      = require('fs'),
    Cromag  = require('cromag'),
    OrestarImporter = require('./orestar_importer.js');

var baseUri = "https://secure.sos.state.or.us/orestar/";
var requestUri = function(startDate) {
  var now = new Cromag();
  if (startDate === null) { startDate = now.addDays(-7); }

  return baseUri + "gotoPublicTransactionSearchResults.do?" +
    "cneSearchButtonName=search&cneSearchFilerCommitteeId=*&cneSearchTranEndDate=" +
    now.toFormat("MM/DD/YYYY") + "&cneSearchTranStartDate=" + startDate.toFormat("MM/DD/YYYY");
};

request({
  uri: requestUri(null),
}, function(error, response, body) {
  if (error || response.statusCode !== 200) {
    console.log("Unexpected response/Error: " + error);
  } else {
    var sourceFile = './downloads/orestar_data.xls';
    var writeFile = fs.createWriteStream(sourceFile);

    request({
      uri: baseUri + "XcelCNESearch",
      headers: { cookie: response.headers['set-cookie'] }
    }, function(e, res, b) {
      if (e) {
        console.log(e);
      } else if (res.headers['content-type'] !== 'application/vnd.ms-excel') {
        console.log("Incorrect file type detected. " + res.headers['content-type']);
      }
    }).pipe(writeFile);

    writeFile.on('close', function() {
      // TODO: store data in database
      OrestarImporter(this.path);

      var archivedFile = './downloads/archived/data-' + Date.now().toString() + '.csv';
      fs.rename(this.path, archivedFile, function(e) { if (e) { console.log(e); } });
      // TODO: clean this up if we get more than x files
    });
  }
});

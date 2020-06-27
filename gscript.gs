/*
//https://script.google.com/macros/s/AKfycbymWSSX2il-aUSc0ZCa59Jk8o_yEEZPUvdFI-OkiZELSzY71viH/exec?temperature=108.6&status=AbNormal&RFIDhexcode=R19CA616
// Description: This code for demonstration send data from ESP8266 into Google Spreadsheet

*/

function doGet(e) { 
  Logger.log( JSON.stringify(e) );  // view parameters
  var result = 'Ok'; // assume success
  if (e.parameter == 'undefined') {
    result = 'No Parameters';
  }
  else {
    var sheet_id = '1GqGAosnaoI7rNcK-8aAlBeNfBN2NPEobu75wRgjpuzc'; 		// Spreadsheet ID
    var sheet = SpreadsheetApp.openById(sheet_id).getActiveSheet();		// get Active sheet
    var newRow = sheet.getLastRow() + 1;						
    var rowData = [];
    rowData[0] =new Date();
    var d = new Date();
    var tm = d.toLocaleTimeString();
    rowData[1] =tm;
    for (var param in e.parameter) {
      Logger.log('In for loop, param=' + param);
      var value = stripQuotes(e.parameter[param]);
      Logger.log(param + ':' + e.parameter[param]);
      switch (param) {
        case 'temperature': //Parameter
          rowData[2] = value + ' F'; //Value in column C
          result = 'Written on column C\n';
          break;
          case 'status': //Parameter
          rowData[3] = value; //Value in column D
          result = 'Written on column D\n';
          break;
        case 'RFIDhexcode': //Parameter
          rowData[4] = value; //Value in column E
          result += 'Written on column E\n';
          break;
        default:
          result = "unsupported parameter";
      }
    }
    Logger.log(JSON.stringify(rowData));
    // Write new row below
    var newRange = sheet.getRange(newRow, 1, 1, rowData.length);
    newRange.setValues([rowData]);
  }
  // Return result of operation
  return ContentService.createTextOutput(result);
}
/*
Remove leading and trailing single or double quotes
*/
function stripQuotes( value ) {
  return value.replace(/^["']|['"]$/g, "");
}
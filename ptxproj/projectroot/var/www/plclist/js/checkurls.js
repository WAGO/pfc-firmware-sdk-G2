$(document).ready(function(){

  $('#plcselection a').map(function(){
    var testUrl = $(this).attr('href'),
		
        called_urldata = window.location.href.split('/'),
        called_baseUrl = called_urldata[0] + '//' + called_urldata[2],
        self = this;
    testUrl = testUrl.replace(/http:\/\/localhost/, called_baseUrl);
	$(this).attr('href', testUrl);
    console.log(testUrl);
    $.ajax({
      url: testUrl,
      error: function() {
        $(self).addClass('disabled');
        $(self).removeAttr('href');
      }
    });
  })
});

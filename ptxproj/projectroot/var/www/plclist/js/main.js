
$(document).ready(function(){
  
  
  // retrieving the original url
  var testUrl = $(this).attr('href'),
	called_urldata = window.location.href.split('/'),
	called_baseUrl = called_urldata[0] + '//' + called_urldata[2];
  
  /**  
   * the link click behaviour should be replaced by a
   * config-tools call to start the browser with the given url
   * and the virtual keyboard enabled if nessecary
   */
  $('#plcselection a').click(function(){
    var params = JSON.stringify({
        0: {
          name: 'start_browser',
          parameter: [
            'url=' + $(this).attr('href'),
			'vkeyb=' + $(this).data('vkb')
          ],
          dataId:  0,
          sudo: true,
          timeout: 10000,
          multiline: false
        }
    });

    $.ajax({
      type: 'POST',
      url: called_baseUrl + '/plclist/configtools.php',
      dataType: 'JSON',
      data: params
    });

    return false;
  });
  
  $('.content').height($(window).height()-50);
  
})

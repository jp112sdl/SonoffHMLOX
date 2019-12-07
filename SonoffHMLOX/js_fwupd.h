const char WM_HTTP_CUSTOMUPDATESCRIPT[] PROGMEM = R"=====(
function updateCheck() { 
  var json_obj = JSON.parse(Get('{fwjsurl}')); 
  if (json_obj.tag_name != '{fw}') {
    document.getElementById('fwbtn').style.display='block';
    document.getElementById('fwbtn').title = json_obj.tag_name; 
    document.getElementById('fwbtnupdt').style.display='block';
    document.getElementById('fwbtnupdt').title = json_obj.tag_name; 
  } 
}; 

setTimeout(function(){ updateCheck() }, 3000); 

)=====";

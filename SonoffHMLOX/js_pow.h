const char WM_HTTP_CUSTOMPOWSCRIPT[] PROGMEM = R"=====(
setTimeout(function(){ refresh(); }, {mi}); 

function refresh() { 
  var json_obj = JSON.parse(Get('/getPowerJSON')); 
  document.getElementById('_v').innerHTML = json_obj.Voltage; 
  document.getElementById('_c').innerHTML = json_obj.Current; 
  document.getElementById('_w').innerHTML = json_obj.PowerW; 
  document.getElementById('_va').innerHTML = json_obj.PowerVA; 
  document.getElementById('_ec').innerHTML = json_obj.EnergyCounter; 
  setTimeout(function(){ refresh() }, {mi}); 
} 
)=====";

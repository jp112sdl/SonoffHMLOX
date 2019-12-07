const char WM_HTTP_CUSTOMSCRIPT[] PROGMEM = R"=====(
var timerRun = false; 

function Get(u){ 
  var h = new XMLHttpRequest(); 
  h.open('GET',u,false); 
  h.send(null); 
  return h.responseText; 
} 

function SetState(v) { 
  document.getElementById('timer').value = ''; 
  var json_obj = JSON.parse(Get(v)); 
  refreshState(json_obj, false); 
} 

function isInt(v){
  return !isNaN(v) && parseInt(Number(v))==v && !isNaN(parseInt(v,10));
}

function timerdecrement(t,rekursiv) { 
  if (rekursiv) timerRun=false; 
  if (isInt(t.placeholder)) {
    t.placeholder = t.placeholder-1; 
    if (t.placeholder > 0 && !timerRun) timerRun = setTimeout(function(){ timerdecrement(t, true) }, 1000); 
    else setTimeout(function(){ refreshState(null, false);}, 1000);  
  }
} 

function refreshState(json_obj, rekursiv) { 
  if (json_obj == null) json_obj = JSON.parse(Get('/getState')); 
  document.getElementById('_ls').innerHTML = (('1' == json_obj.state) ? 'AN' : 'AUS'); 
  var timer = document.getElementById('timer'); 
  timer.placeholder = json_obj.resttimer; 
  if (timer.placeholder == 0) timer.placeholder = 'Sekunden'; 
  else timerdecrement(timer); 
  if (rekursiv) setTimeout(function(){ refreshState(null, true); }, 3000); 
} 

/*init refresh:*/ 
refreshState(null, true); 

)=====";

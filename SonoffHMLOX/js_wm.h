const char WM_HTTP_SCRIPT[] PROGMEM = R"=====(
<script>
  function c(l) { 
    document.getElementById('s').value = l.innerText || l.textContent; document.getElementById('p').focus();
  }
  
  function setBackendType() {
    var backendtype = document.getElementById('backendtype'); 
    var ccu = document.getElementById('ccu'); 
    var qsa = document.querySelectorAll('[id^=div_]'); 
    qsa.forEach(function(e) { 
      e.style.display = 'block'; 
      }); 
      
    qsa = document.querySelectorAll('[id$=_pow]'); 
    qsa.forEach(function(e) { 
      e.style.display = 'block'; 
      });
       
    qsa = document.querySelectorAll('[id$=_switch]'); 
    qsa.forEach(function(e) { 
      e.style.display = 'block'; 
      }); 
        
    if (backendtype) { 
      var flt; 
      switch (parseInt(backendtype.value)) { 
        case 0: 
          flt = 'lox'; 
          if (ccu) ccu.placeholder = 'IP der CCU2'; 
          break; 
        case 1: 
          flt = 'hm'; 
          if (ccu) ccu.placeholder = 'IP des LOXONE Miniservers'; 
          break; 
      } 
      qsa = document.querySelectorAll('[id^=div_' + flt + ']'); 
      qsa.forEach(function(e) { e.style.display = 'none'; }); 
    } 
    
    if (sonoffmodel) { 
      switch (parseInt(sonoffmodel.value)) { 
        case 0: 
          qsa = document.querySelectorAll('[id$=_pow]'); 
          qsa.forEach(function(e) { e.style.display = 'none'; }); 
          break; 
        case 1: 
          qsa = document.querySelectorAll('[id$=_switch]'); 
          qsa.forEach(function(e) { e.style.display = 'none'; }); 
          break; 
        case 2: 
          qsa = document.querySelectorAll('[id$=_pow]'); 
          qsa.forEach(function(e) { e.style.display = 'none'; }); 
          break; 
        case 3: 
          qsa = document.querySelectorAll('[id$=_pow]'); 
          qsa.forEach(function(e) { e.style.display = 'none'; }); 
          break; 
      } 
    }
  }
</script>
)=====";

const char WM_HTTP_ALL_STYLE[] PROGMEM = R"=====(
<style>
  div {
    white-space: nowrap;
  } 
  
  input[type=text] {
    width:95%;
  } 
  
  .green {
    color:green;
  } 
  
  .red {
    color:red;
  } 
  
  .tdr {
    text-align: right;
  } 
  
  .tdl {
    width: 1px;
  } 

 .ckb{
    float: right;
    width: 18px;
    text-align: right;
  } 
  
  .cob { 
    border: 1px solid #ccc; 
    width: 180px; 
    border-radius: 3px; 
    overflow: hidden; 
    background: #fafafa no-repeat 90% 50%;
  } 
  
  .cob select { 
    padding: 5px 8px; 
    border: none; 
    box-shadow: none; 
    background: transparent; 
    background-image: none; 
    webkit-appearance: none; 
    font-size: 1em;
  }
  
  .cob select:focus { 
    outline: none;
  } 
  
  input.lnkbtn,input.fwbtn {
    -webkit-appearance: button;
    -moz-appearance: button;
    appearance: button;
  } 
  
  body {
    background-color: #303030;
  } 
  
  input.lnkbtn,button,input.fwbtn{
    cursor: pointer;
    color:#fff;
    line-height:2.4rem;
    font-size:1.2rem;
    width:100%;
    padding:5px;
  } 
  
  input,button,input.lnkbtn,input.fwbtn {
    border: 0;
    border-radius: 0.3rem;
  } 
  
  .c{
    text-align: center;
  } 
  
  .k{
    font-style:italic;
  } 
  
  .fbg {
    background-color: #eee;
  } 
  
  div,input{
    padding:5px;
    font-size:1em;
  } 
  
  .i{
    text-align: right; 
    width: 45% !important;
  } 
  
  body{
    text-align: center;
    font-family:verdana;
  } 
  
  .l{
    no-repeat left center;
    background-size: 1em;
  } 
  
  .q{
    float: right;
    width: 64px;
    text-align: right;
  } 
  
  .ls {
    font-weight: bold;
    text-align: center;
    font-size: 300%;
  } 
  
  .lt{
    font-size: 150%;
    text-align: center;
  }
    
  table{
    width:100%;
  } 
  
  td{
    max-width:50%;
    font-weight: bold;
  } 
  
  input.fwbtn {
    display: none; 
    background-color: #ff0000;
  } 
</style>
)=====";

const char WM_HTTP_HM_STYLE[]  PROGMEM = R"=====(
<style>
  input.lnkbtn,button{
    background-color:#1fa3ec;
  }
</style>
)=====";

const char WM_HTTP_LOX_STYLE[] PROGMEM = R"=====(
<style>
  input.lnkbtn,button{
    background-color:#83b817;
  }
</style>
)=====";

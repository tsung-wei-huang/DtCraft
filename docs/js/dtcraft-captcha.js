var allowSubmit = false;
function captcha_filled () {
  allowSubmit = true;
}
function captcha_expired () {
  allowSubmit = false;
}
function check_if_captcha_is_filled (e) {
  if(allowSubmit){
     alert('Your feedback has been sent. Thanks!');
     return true;
  }   
  alert('Please check the CAPTCHA!');
  return false;
//  e.preventDefault();
}


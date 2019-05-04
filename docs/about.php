<?php 
if(isset($_POST['submit']) && !empty($_POST['submit'])):
  if(isset($_POST['g-recaptcha-response']) && !empty($_POST['g-recaptcha-response'])):
    //your site secret key
    $secret = '6LerYjsUAAAAAAogGUkYnJpOSoIN_aaoUFng4aYx';
    //get verify response data
    $verifyResponse = file_get_contents('https://www.google.com/recaptcha/api/siteverify?secret='.$secret.'&response='.$_POST['g-recaptcha-response']);
    $responseData = json_decode($verifyResponse);
    if($responseData->success):
      $myemail = 'clin99@illinois.edu';//<-----Put Your email address here.
      
      $name = $_POST['fname']; 
      $email_address = $_POST['email']; 
      $subject = $_POST['subject']; 
      $message = $_POST['message']; 
      
      $to = $myemail; 
      $email_subject = "Contact form submission: $name";
      $email_body = "You have received a new message. ".
      " Here are the details:\n\n Name: $name \n Email: $email_address \n Subject: $subject \n Message: \n $message"; 
      
      $headers = "From: $myemail\n"; 
      $headers .= "Reply-To: $email_address";
      
      mail($to,$email_subject,$email_body,$headers);
    endif;
  endif;
endif;

//redirect to the 'thank you' page
header("Location: http://dtcraft.web.engr.illinois.edu/about.html");
exit; 
?>

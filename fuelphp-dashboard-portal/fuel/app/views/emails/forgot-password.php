<?php echo View::forge('emails/email-header'); ?>

We have received a reset password request for your account:
<br/><br/>
<a href="<?php echo $url; ?>"><?php echo $url; ?></a>

<br/><br/>
Please contact our team if you believe this is a mistake!

<?php echo View::forge('emails/email-footer'); ?>
<form method="post" class="text-center" action="<?php echo Uri::create('auth'); ?>">
  <input type="text" class="form-control" name="username" placeholder="Username" required value="<?php echo Input::post('username'); ?>" />
  <input type="password" class="form-control" name="password" required placeholder="Password" />
  <button type="submit" class="btn btn-success">Login</button>
</form>
<hr/>
<div class="row text-center">
  <div class="col-sm-6">
      <a href="<?php echo Uri::create('auth/registration'); ?>">create an account</a>
  </div>
  <div class="col-sm-6">
      <a href="<?php echo Uri::create('auth/reset'); ?>">forgot password</a>
  </div>
</div>

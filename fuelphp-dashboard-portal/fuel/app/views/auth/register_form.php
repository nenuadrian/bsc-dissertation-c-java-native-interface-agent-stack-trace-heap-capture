<form method="post" class="text-center" action="<?php echo Uri::create('auth/register'); ?>">
  <input type="text" class="form-control" name="username" placeholder="Username" value="<?php echo Input::post('username'); ?>" required />
  <input type="email" class="form-control" name="email" placeholder="E-mail" value="<?php echo Input::post('email'); ?>" required />
  <input type="password" class="form-control" name="password" placeholder="Password" required />
  <button type="submit" class="btn btn-success">Register</button>
</form>

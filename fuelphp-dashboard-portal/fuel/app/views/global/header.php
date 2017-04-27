<?php use \Model\Auth; ?>
<!DOCTYPE html>
  <head>
    <meta charset="utf-8">
    <title><?php echo isset($title) ? $title . ' - ' : ''; ?><?php echo Config::get('title'); ?></title>
    <link rel="shortcut icon" href="<?php echo Uri::create('favicon.ico'); ?>" type="image/x-icon" />
    <link href="https://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css" rel="stylesheet">
    <?php echo Asset::css('bootstrap.min.css'); ?>
    <?php echo Asset::css('styles.css'); ?>
    <?php echo Asset::js('jquery.min.js'); ?>

    <?php echo Asset::js('bootstrap.min.js'); ?>
  	<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.4.0/Chart.bundle.min.js"></script>
    <?php echo Asset::js('global.js'); ?>

	</head>
  <body>
	<div class="modal fade" id="authModal" tabindex="-1" role="dialog">
		<div class="modal-dialog" role="document">
			<div class="modal-content">
				<div class="modal-header">
					<h5 class="modal-title">Authenticate</h5>
				</div>
				<div class="modal-body">
					<?php echo View::forge('auth/login_form'); ?>
				</div>
			</div>
		</div>
	</div>
		<div class="modal fade" id="registerModal" tabindex="-1" role="dialog">
		<div class="modal-dialog" role="document">
			<div class="modal-content">
				<div class="modal-header">
					<h5 class="modal-title">Create an account</h5>
				</div>
				<div class="modal-body">
					<?php echo View::forge('auth/register_form'); ?>
				</div>
			</div>
		</div>
	</div>
	<nav class="navbar navbar-default navbar-fixed-top">
      <div class="container">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target="#main-nav" aria-expanded="false">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
        </div>

        <div class="collapse navbar-collapse text-center" id="main-nav">

          <ul class="nav navbar-nav navbar-right">
          <?php if (Session::get('token')): ?>
              <li><a href="<?php echo Uri::create('apps'); ?>">Apps</a></li>
            <?php if (Auth::can('cardinal')): ?>
              <li><a href="<?php echo Uri::create('sponsors'); ?>">Sponsors</a></li>
              <li><a href="<?php echo Uri::create('leagues'); ?>">Leagues</a></li>
            <?php endif; ?>
            <li><a href="<?php echo Uri::create('auth/logout'); ?>">Logout</a></li>
          <?php else: ?>
            <li><a href="<?php echo Uri::base(); ?>">Home</a></li>
             <li><a href="#"  data-toggle="modal" data-target="#authModal">Login</a></li>
             <li><a href="#"  data-toggle="modal" data-target="#registerModal">Register</a></li>
          <?php endif; ?>
            <!--<li><a href="<?php echo Uri::create('contact'); ?>">Contact</a></li>-->

          </ul>
        </div><!-- /.navbar-collapse -->
      </div><!-- /.container-fluid -->
    </nav>

    <div class="container page">
		<?php echo View::forge('global/messages'); ?>

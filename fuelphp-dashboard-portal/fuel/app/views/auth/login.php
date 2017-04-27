<?php echo View::forge('global/header'); ?>
<div class="row">
	<div class="col-md-3">
	</div>
	<div class="col-md-6">
		<div class="panel panel-default text-center">
			<div class="panel-heading">Authenticate</div>
			<div class="panel-body">
				<?php echo View::forge('auth/login_form') ;?>
			</div>
		</div>
	</div>
</div>
<?php echo View::forge('global/footer'); ?>
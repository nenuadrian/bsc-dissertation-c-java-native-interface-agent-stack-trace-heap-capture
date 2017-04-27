<?php echo View::forge('global/header'); ?>
<div class="row">
	<div class="col-md-3">
	</div>
	<div class="col-md-6">
		<div class="panel panel-default text-center">
			<div class="panel-heading">Register</div>
			<div class="panel-body">
				<?php echo View::forge('auth/register_form') ;?>
			</div>
		</div>
	</div>
</div>
<?php echo View::forge('global/footer'); ?>

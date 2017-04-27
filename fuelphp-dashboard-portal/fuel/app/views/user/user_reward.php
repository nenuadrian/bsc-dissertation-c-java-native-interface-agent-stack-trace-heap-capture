<?php use \Model\Team; echo View::forge('global/header'); ?>

<h1 class="text-center"><?php echo $reward['name']; ?></h1>
<hr/>
<div class="row">
  <div class="col-md-4 text-center">
    <img src="<?php echo $reward['image']; ?>" />
  </div>
  <div class="col-md-8">
    <div class="well">
      <?php print_r($reward); ?>
    </div>
  </div>
</div>
<?php echo View::forge('global/footer'); ?>

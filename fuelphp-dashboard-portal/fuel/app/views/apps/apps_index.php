<?php echo View::forge('global/header'); ?>


<div class="text-center">
  <form method="post">
    <button class="btn btn-success" type="submit" name="action" value="add">New app</button>
  </form>
</div>
<Br/>
<?php if (!$apps): ?>
  <div class="alert alert-warning">
    None found
  </div>
<?php endif; ?>

<div class="list-group">
  <?php foreach ($apps as $a): ?>
    <a href="<?php echo Uri::create('apps/app/' . $a['id']); ?>" class="list-group-item">
      <h5 class="list-group-item-heading"><?php echo $a['name']; ?></h5>
      <p class="list-group-item-text">Hash: <?php echo $a['id']; ?> - <?php echo $a['created_at']; ?></p>
    </a>
  <?php endforeach; ?>

</div>
<?php echo View::forge('global/footer'); ?>

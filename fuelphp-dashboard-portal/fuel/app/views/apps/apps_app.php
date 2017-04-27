<?php echo View::forge('global/header'); ?>

  <div class="panel panel-default">
    <div class="panel-heading">
      <?php echo $app['name']; ?>
    </div>
    <div class="panel-body">
      <form class="form-horizontal" method="post">
        <div class="form-group">
          <label class="col-sm-2 control-label">Name</label>
          <div class="col-sm-10">
            <input type="text" name="name" class="form-control" placeholder="Name" value="<?php echo Input::post('name', $app['name']); ?>"/>
          </div>
        </div>
        <div class="form-group">
          <label class="col-sm-2 control-label">Hash</label>
          <div class="col-sm-10">
            <input type="text" disabled class="form-control" value="<?php echo $app['id']; ?>"/>
          </div>
        </div>
        <button type="submit" class="btn btn-default btn-block" name="action" value="update">Update</button>
      </form>
    </div>
  </div>
  <?php if (!$logs): ?>
    <div class="alert alert-info">
      No errors have been registered in this app
    </div>

  <?php endif; ?>
  <div class="list-group">
    <?php foreach($logs as $log): ?>
        <a href="<?php echo Uri::create('apps/log/' . $app['id'] . '/' . $log['id']); ?>" class="list-group-item">
          <h4 class="list-group-item-heading"><?php echo $log['summary']; ?></h4>
          <p class="list-group-item-text"><?php echo $log['created_at']; ?></p>
        </a>
    <?php endforeach; ?>
  </div>

<?php echo View::forge('global/footer'); ?>

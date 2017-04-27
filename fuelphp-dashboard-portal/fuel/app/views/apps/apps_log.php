<?php echo View::forge('global/header'); ?>

<h3 class="text-center"><?php echo $log['summary']; ?></h3>
<?php print_r($log['message']['ex']['caught']); ?>
<hr/>
<?php
  $objects = array();
  foreach ($log['message']['ex']['objects']['v'] as $k => $o) {
    $objects[$o['@attributes']['id']] = $o;
    unset($log['message']['ex']['objects'][$k]);
  }

?>
<div class="row">
  <div class="col-md-4">
    <div class="list-group">
      <?php foreach ($log['message']['ex']['sts']['st'] as $k => $st): ?>
        <a href="<?php echo Uri::create('apps/log/' . $app_id . '/' . $log['id'] . '/' . $k); ?>" class="list-group-item <?php echo $selected_st == $k ? 'active' : ''; ?>">
          <h5 class="list-group-item-heading"><?php echo $st['m']; ?></h5>
          <p class="list-group-item-text"><?php echo $st['s']; ?></p>
          <p class="list-group-item-text"><?php echo $st['sig']; ?></p>
        </a>
      <?php endforeach; ?>
    </div>
  </div>
  <div class="col-md-8">
    <div class="panel panel-default">
      <div class="panel-body">
        <?php $st = $log['message']['ex']['sts']['st'][$selected_st]; ?>
        <?php foreach ($st['vars']['v'] as $v): $o = isset($v['@attributes']['id']) ? $objects[$v['@attributes']['id']] : $v;
          $o = isset($v['id']) ? $objects[$v['id']] : $o; ?>
          <h4><?php echo $o['n']; ?>: <?php echo $o['s']; ?></h4>
          <?php if (isset($o['a_l'])): ?>
            <p>Array length: <?php echo $o['a_l']; ?></p>
          <?php endif; ?>
          <?php if (isset($o['v'])): ?>
            <p>Value: <?php echo $o['v']; ?></p>
          <?php endif; ?>
        <?php endforeach; ?>
        <?php //print_r($st);
        ?>
      </div>
    </div>
  </div>
</div>
<?php //print_r($log['message']['ex']);
?>
<?php echo View::forge('global/footer'); ?>

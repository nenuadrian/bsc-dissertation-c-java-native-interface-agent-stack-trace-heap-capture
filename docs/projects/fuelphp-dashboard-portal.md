# FuelPHP Dashboard Portal

Path: `fuelphp-dashboard-portal/`

## Purpose

Web dashboard that visualizes application diagnostics, exceptions, and captured state produced by the agent and API stack.

## Stack

- FuelPHP `1.8`
- PHP `>=5.3.3`
- Composer-managed dependencies
- Bootstrap/jQuery front-end assets

## Key Files

- `composer.json`: PHP framework and package dependencies.
- `public_html/`: Front-end entrypoint and static assets.
- `fuel/`: FuelPHP core/packages/app layers.

## Typical Commands

Install dependencies:

```bash
cd fuelphp-dashboard-portal
composer install
```

Run through your PHP/FuelPHP setup (Apache/Nginx/PHP-FPM or local PHP server) using `public_html/` as web root.

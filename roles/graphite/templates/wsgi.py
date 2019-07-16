import os, sys
sys.path.append('{{graphite_dir}}')

os.environ['DJANGO_SETTINGS_MODULE'] = 'graphite.settings'

try:
    from importlib import import_module
except ImportError:
    from django.utils.importlib import import_module

#
from django.conf import settings
from django.core.wsgi import get_wsgi_application
from graphite.logger import log

application = get_wsgi_application()

#import django.core.handlers.wsgi
#application = django.core.handlers.wsgi.WSGIHandler()

# READ THIS
# Initializing the search index can be very expensive, please include
# the WSGIImportScript directive pointing to this script in your vhost
# config to ensure the index is preloaded before any requests are handed
# to the process.
from graphite.logger import log
log.info("graphite.wsgi - pid %d - reloading search index" % os.getpid())
import graphite.metrics.search

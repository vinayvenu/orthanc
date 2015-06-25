## Introduction ##

This page explains how to replicate the content of one instance of Orthanc to another instance of Orthanc. This can be useful to upgrade between versions of the database schema, or to create mirrored copies.

## Direct Access to the Filesystem ##

The most direct way to replicate consists in using the [ImportDicomFiles](https://code.google.com/p/orthanc/source/browse/Resources/Samples/ImportDicomFiles/ImportDicomFiles.py) script of the Orthanc distribution. For instance, the following command would recursively explore the content of the `OrthancStorage` folder (where Orthanc stores its DICOM files by default), and send each DICOM file inside this folder to the instance of Orthanc whose REST API is listening on 192.168.0.2:8042:

```
# python ImportDicomFiles.py 192.168.0.2 8042 OrthancStorage
```

This method will only succeed if:
  * The source Orthanc uses the default SQLite back-end of Orthanc (and not the [PostgreSQL plugin](https://code.google.com/p/orthanc-postgresql/), for instance),
  * You have command-line access to the source Orthanc, and
  * The transparent compression of the DICOM instances is disabled (cf. option `StorageCompression` in the configuration file).


## Generic Replication ##

If you cannot use the first method, you can use the [Replicate](https://code.google.com/p/orthanc/source/browse/Resources/Samples/Python/Replicate.py) script of the Orthanc distribution. This script will use the REST API of both the source and target instances of Orthanc. For instance:

```
# python Replicate.py http://orthanc:password@localhost:8042/ http://192.168.0.2/
```

Obviously, contrarily to the first method, the source instance of Orthanc must be up and running during the replication.
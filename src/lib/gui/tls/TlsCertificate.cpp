/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025 Deskflow Developers
 * SPDX-FileCopyrightText: (C) 2015 Symless Ltd.
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "TlsCertificate.h"

#include "base/FinalAction.h"
#include "common/Settings.h"
#include "io/Filesystem.h"
#include "net/Fingerprint.h"
#include "net/FingerprintDatabase.h"
#include "net/SecureUtils.h"

#include <QDir>
#include <QProcess>

#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/x509.h>

TlsCertificate::TlsCertificate(QObject *parent) : QObject(parent)
{
  // do nothing
}

bool TlsCertificate::generateCertificate(const QString &path, int keyLength) const
{
  qDebug("generating tls certificate: %s", qUtf8Printable(path));

  QFileInfo info(path);
  if (QDir dir(info.absolutePath()); !dir.exists() && !dir.mkpath(".")) {
    qCritical("failed to create directory for tls certificate");
    return false;
  }

  try {
    deskflow::generatePemSelfSignedCert(path.toStdString(), keyLength);
  } catch (const std::exception &e) {
    qCritical() << "failed to generate self-signed pem cert: " << e.what();
    return false;
  }
  qDebug("tls certificate generated");
  return generateFingerprint(path);
}

bool TlsCertificate::generateFingerprint(const QString &certificateFilename) const
{
  qDebug("generating tls fingerprint");
  const std::string certPath = certificateFilename.toStdString();
  FingerprintDatabase db;
  db.addTrusted(deskflow::pemFileCertFingerprint(certPath, Fingerprint::Type::SHA256));
  return db.write(Settings::tlsLocalDb());
}

int TlsCertificate::getCertKeyLength(const QString &path) const
{
  return deskflow::getCertLength(path.toStdString());
}

bool TlsCertificate::isCertificateValid(const QString &path) const
{
  OpenSSL_add_all_algorithms();
  ERR_load_crypto_strings();

  auto fp = deskflow::fopenUtf8Path(path.toStdString(), "r");
  if (!fp) {
    qWarning() << tr("could not read from default certificate file");
    return false;
  }
  auto fileClose = deskflow::finally([fp]() { std::fclose(fp); });

  auto *cert = PEM_read_X509(fp, nullptr, nullptr, nullptr);
  if (!cert) {
    qWarning() << tr("could not load default certificate file to memory");
    return false;
  }
  auto certFree = deskflow::finally([cert]() { X509_free(cert); });

  auto *pubkey = X509_get_pubkey(cert);
  if (!pubkey) {
    qWarning() << tr("default certificate key file does not contain valid public key");
    return false;
  }
  auto pubkeyFree = deskflow::finally([pubkey]() { EVP_PKEY_free(pubkey); });

  if (auto type = EVP_PKEY_type(EVP_PKEY_id(pubkey)); type != EVP_PKEY_RSA && type != EVP_PKEY_DSA) {
    qWarning() << tr("public key in default certificate key file is not RSA or DSA");
    return false;
  }

  if (EVP_PKEY_bits(pubkey) < 2048) {
    // We could have small keys in old barrier installations
    qWarning() << tr("public key in default certificate key file is too small");
    return false;
  }

  return true;
}

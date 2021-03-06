#include "libernet/AddressHistory.h"

#define dotest(condition)                                                      \
  if (!(condition)) {                                                          \
    fprintf(stderr, "FAIL(%s:%d)'%s'->'%s': %s\n", __FILE__, __LINE__,         \
            addresses[j], addresses[j + 1], #condition);                       \
  }

int main(const int /*argc*/, const char *const /*argv*/[]) {
  int iterations = 2;
#ifdef __Tracer_h__
  iterations = 1;
#endif
  try {
    const char *addresses[] = {
        " \\ test \\ me \\ now \\ ",
        "TEST / ME / NOW", // case, spaces, backslash
        " / ",
        " ", // root path, space, empty path
        " / test/me/now/",
        "Test/Me/Now \t\r\n", // trailing /, various whitespace
    };
    data::AddressHistory::List l1, l2;

    for (int i = 0; i < iterations; ++i) {

      for (int j = 0; j < int(sizeof(addresses) / sizeof(addresses[0]));
           j += 2) {
        data::AddressHistory h1, h2;
        data::Data d1(addresses[j], data::Data::Encrypted);
        data::Data d2(addresses[j + 1], data::Data::Encrypted);
        data::Data d3(std::string(addresses[j]) + addresses[j + 1],
                      data::Data::Encrypted);
        data::Data d4(std::string(addresses[j + 1]) + addresses[j],
                      data::Data::Encrypted);
        data::Data d5(std::string(addresses[j]) + addresses[j],
                      data::Data::Encrypted);
        dt::DateTime start;

        h1.setAddress(addresses[j]);
        h2.setAddress(addresses[j + 1]);
        dotest(h1.address() != h2.address());

        h1.calculate(1);
        h2.calculate(1);

        dotest(h1.match() >= 1);
        dotest(h2.match() >= 1);
        dotest(data::AddressHistory(h1).match() ==
               data::AddressHistory(h1).match());
        dotest(h1.bundleCount() == 0);
        dotest(h1.bundles(l1).size() == h2.bundles(l2).size());
        dotest(l1.size() == 0);

        h1.append(d1, "", "", start);
        h1.append(d2, "", "", start + 1.0);
        dotest(h1.bundleCount() == 2);
        dotest(h1.bundles(l1).size() == 2);
        dotest(l1[0] == d1.identifier());
        dotest(l1[1] == d2.identifier());

        h2 = h1;
        dotest(h2.bundleCount() == 2);
        dotest(h2.bundles(l2).size() == 2);
        dotest(l2[0] == d1.identifier());
        dotest(l2[1] == d2.identifier());

        h1.insert(0, d3, "", "", start + 2.0);
        h1.insert(h1.bundleCount(), d4, "", "", start + 3.0);
        dotest(h1.bundleCount() == 4);
        dotest(h1.bundles(l1).size() == 4);
        // printf("h1.bundles.size() == %ld\n", l1.size());
        dotest(l1[0] == d3.identifier());
        dotest(l1[1] == d1.identifier());
        dotest(l1[2] == d2.identifier());
        dotest(l1[3] == d4.identifier());

        h2 = h1;
        dotest(h2.bundleCount() == 4);
        dotest(h2.bundles(l2).size() == 4);
        dotest(l2[0] == d3.identifier());
        dotest(l2[1] == d1.identifier());
        dotest(l2[2] == d2.identifier());
        dotest(l2[3] == d4.identifier());

        h1.insert(d1.identifier(), d5, "", "", start + 4.0);
        dotest(h1.bundleCount() == 5);
        dotest(h1.bundles(l1).size() == 5);
        // printf("l1[0]'%s' == d3'%s'\n", l1[0].c_str(),
        // d3.identifier().c_str()); printf("l1[1]'%s' == d5'%s'\n",
        // l1[1].c_str(), d5.identifier().c_str()); printf("l1[2]'%s' ==
        // d1'%s'\n", l1[2].c_str(), d1.identifier().c_str()); printf("l1[3]'%s'
        // == d2'%s'\n", l1[3].c_str(), d2.identifier().c_str());
        // printf("l1[4]'%s' == d4'%s'\n", l1[4].c_str(),
        // d4.identifier().c_str());
        dotest(l1[0] == d3.identifier());
        dotest(l1[1] == d5.identifier());
        dotest(l1[2] == d1.identifier());
        dotest(l1[3] == d2.identifier());
        dotest(l1[4] == d4.identifier());

        h2 = h1;
        dotest(h2.bundleCount() == 5);
        dotest(h2.bundles(l2).size() == 5);
        dotest(l2[0] == d3.identifier());
        dotest(l2[1] == d5.identifier());
        dotest(l2[2] == d1.identifier());
        dotest(l2[3] == d2.identifier());
        dotest(l2[4] == d4.identifier());

        dotest(h2.key(0) == d3.key());
        dotest(h2.key(1) == d5.key());
        dotest(h2.key(2) == d1.key());
        dotest(h2.key(3) == d2.key());
        dotest(h2.key(4) == d4.key());

        dotest(h2.key(d1.identifier()) == d1.key());
        dotest(h2.key(d2.identifier()) == d2.key());
        dotest(h2.key(d3.identifier()) == d3.key());
        dotest(h2.key(d4.identifier()) == d4.key());
        dotest(h2.key(d5.identifier()) == d5.key());

        /*
printf("start = %0.3f\n", start.seconds());
printf("h2[d1] = %0.3f\n", h2.timestamp(d1.identifier()).seconds());
printf("h2[d2] = %0.3f\n", h2.timestamp(d2.identifier()).seconds());
printf("h2[d3] = %0.3f\n", h2.timestamp(d3.identifier()).seconds());
printf("h2[d4] = %0.3f\n", h2.timestamp(d4.identifier()).seconds());
printf("h2[d5] = %0.3f\n", h2.timestamp(d5.identifier()).seconds());

printf("h2[d1] timestamp = %0.3f seconds\n",
       h2.timestamp(d1.identifier()) - start);
printf("h2[d2] timestamp = %0.3f seconds\n",
       h2.timestamp(d2.identifier()) - start);
printf("h2[d3] timestamp = %0.3f seconds\n",
       h2.timestamp(d3.identifier()) - start);
printf("h2[d4] timestamp = %0.3f seconds\n",
       h2.timestamp(d4.identifier()) - start);
printf("h2[d5] timestamp = %0.3f seconds\n",
       h2.timestamp(d5.identifier()) - start);
    */
        dotest(h2.timestamp(d1.identifier()) - start < 1.0);
        dotest(h2.timestamp(d2.identifier()) - start - 1.0 < 1.0);
        dotest(h2.timestamp(d3.identifier()) - start - 2.0 < 1.0);
        dotest(h2.timestamp(d4.identifier()) - start - 3.0 < 1.0);
        dotest(h2.timestamp(d5.identifier()) - start - 4.0 < 1.0);

        dotest(h2.signatureCount(d1.identifier()) == 0);
        dotest(h2.signatureCount(d2.identifier()) == 0);
        dotest(h2.signatureCount(d3.identifier()) == 0);
        dotest(h2.signatureCount(d4.identifier()) == 0);
        dotest(h2.signatureCount(d5.identifier()) == 0);

        dotest(h2.blockCount(d1.identifier()) == 0);
        dotest(h2.blockCount(d2.identifier()) == 0);
        dotest(h2.blockCount(d3.identifier()) == 0);
        dotest(h2.blockCount(d4.identifier()) == 0);
        dotest(h2.blockCount(d5.identifier()) == 0);

        h1.sign(d1.identifier(), d2.identifier(),
                text::base64Encode(d3.data()));
        h1.sign(d1.identifier(), d4.identifier(),
                text::base64Encode(d5.data()));
        h1.sign(d1.identifier(), d3.identifier(),
                text::base64Encode(d1.data()));

        h1.block(d1.identifier(), d2.identifier(),
                 text::base64Encode(d3.data()), "reason1");
        h1.block(d1.identifier(), d4.identifier(),
                 text::base64Encode(d5.data()), "reason2");
        h1.block(d1.identifier(), d5.identifier(),
                 text::base64Encode(d1.data()), "reason3");
        h1.block(d1.identifier(), d1.identifier(),
                 text::base64Encode(d1.data()), "reason6");

        h1.sign(d3.identifier(), d3.identifier(),
                text::base64Encode(d2.data()));
        h1.block(d3.identifier(), d5.identifier(),
                 text::base64Encode(d4.data()), "reason4");

        h1.sign(d4.identifier(), d2.identifier(),
                text::base64Encode(d2.data()));
        h1.block(d4.identifier(), d4.identifier(),
                 text::base64Encode(d4.data()), "reason5");

        h2 = h1;
        dotest(h2.signatureCount(d1.identifier()) == 3);
        dotest(h2.signatureCount(d2.identifier()) == 0);
        dotest(h2.signatureCount(d3.identifier()) == 1);
        dotest(h2.signatureCount(d4.identifier()) == 1);
        dotest(h2.signatureCount(d5.identifier()) == 0);

        dotest(h2.signers(d3.identifier(), l2).size() == 1);
        dotest(l2[0] == d3.identifier());
        dotest(h2.signers(d4.identifier(), l2).size() == 1);
        dotest(l2[0] == d2.identifier());

        dotest(h2.signature(d1.identifier(), d2.identifier()) ==
               text::base64Encode(d3.data()));
        dotest(h2.signature(d1.identifier(), d4.identifier()) ==
               text::base64Encode(d5.data()));
        dotest(h2.signature(d1.identifier(), d3.identifier()) ==
               text::base64Encode(d1.data()));
        dotest(h2.signature(d3.identifier(), d3.identifier()) ==
               text::base64Encode(d2.data()));
        dotest(h2.signature(d4.identifier(), d2.identifier()) ==
               text::base64Encode(d2.data()));

        dotest(h2.blockCount(d1.identifier()) == 4);
        dotest(h2.blockCount(d2.identifier()) == 0);
        dotest(h2.blockCount(d3.identifier()) == 1);
        dotest(h2.blockCount(d4.identifier()) == 1);
        dotest(h2.blockCount(d5.identifier()) == 0);

        dotest(h2.blockers(d3.identifier(), l2).size() == 1);
        dotest(l2[0] == d5.identifier());
        dotest(h2.blockers(d4.identifier(), l2).size() == 1);
        dotest(l2[0] == d4.identifier());

        dotest(h2.blockSignature(d1.identifier(), d2.identifier()) ==
               text::base64Encode(d3.data()));
        dotest(h2.blockSignature(d1.identifier(), d4.identifier()) ==
               text::base64Encode(d5.data()));
        dotest(h2.blockSignature(d1.identifier(), d5.identifier()) ==
               text::base64Encode(d1.data()));
        dotest(h2.blockSignature(d1.identifier(), d1.identifier()) ==
               text::base64Encode(d1.data()));
        dotest(h2.blockSignature(d3.identifier(), d5.identifier()) ==
               text::base64Encode(d4.data()));
        dotest(h2.blockSignature(d4.identifier(), d4.identifier()) ==
               text::base64Encode(d4.data()));

        dotest(h2.blockReason(d1.identifier(), d2.identifier()) == "reason1");
        dotest(h2.blockReason(d1.identifier(), d4.identifier()) == "reason2");
        dotest(h2.blockReason(d1.identifier(), d5.identifier()) == "reason3");
        dotest(h2.blockReason(d1.identifier(), d1.identifier()) == "reason6");
        dotest(h2.blockReason(d3.identifier(), d5.identifier()) == "reason4");
        dotest(h2.blockReason(d4.identifier(), d4.identifier()) == "reason5");

        dotest(data::AddressHistory(h2).bundleCount() == 5);
        dotest(data::AddressHistory(h2).blockReason(
                   d4.identifier(), d4.identifier()) == "reason5");

        data::Data hd2(h2);
        data::AddressHistory reconstituted(hd2.data(), hd2.identifier(),
                                           hd2.key(), addresses[j]);
        data::AddressHistory recreated;

        recreated.assign(hd2.data(), hd2.identifier(), hd2.key(),
                         addresses[j + 1]);
        dotest(data::AddressHistory(reconstituted).bundleCount() == 5);
        dotest(data::AddressHistory(reconstituted)
                   .blockReason(d4.identifier(), d4.identifier()) == "reason5");
        dotest(data::AddressHistory(recreated).bundleCount() == 5);
        dotest(data::AddressHistory(recreated).blockReason(
                   d4.identifier(), d4.identifier()) == "reason5");

        h1.remove(3);                    // d2
        h1.remove(h1.bundleCount() - 1); // d4
        h1.remove(0);                    // d3
        dotest(h1.bundleCount() == 2);
        dotest(h1.bundles(l1).size() == 2);
        dotest(l1[0] == d5.identifier());
        dotest(l1[1] == d1.identifier());

        h2.remove(d2.identifier());
        h2.remove(d4.identifier());
        h2.remove(d3.identifier());
        dotest(h2.bundleCount() == 2);
        dotest(h2.bundles(l2).size() == 2);
        dotest(l2[0] == d5.identifier());
        dotest(l2[1] == d1.identifier());

        data::AddressHistory h3;

        h3.setAddress(addresses[j]);
        h3.append(d1, "username", "password");
        dotest(h3.hasUsername(d1.identifier(), "username"));
        dotest(!h3.hasUsername(d1.identifier(), "john"));

        h3.insert(d1.identifier(), d2.identifier(), d2.key(), "please",
                  "thank you");
        dotest(h3.hasUsername(d2.identifier(), "please"));
        dotest(h3.key(d2.identifier(), "please", "thank you") == d2.key());
        dotest(!h3.hasKey(d2.identifier()));

        h3.setPassword(d1.identifier(), d1.key(), "john", "john password");
        dotest(h3.hasUsername(d1.identifier(), "john"));
        dotest(h3.key(d1.identifier(), "username", "password") == d1.key());
        dotest(h3.key(d1.identifier(), "john", "john password") == d1.key());
        dotest(!h3.hasKey(d1.identifier()));

        h3.removePassword(d1.identifier(), d1.key(), "username");
        dotest(!h3.hasKey(d1.identifier()));
        dotest(!h3.hasUsername(d1.identifier(), "username"));
        dotest(h3.hasUsername(d1.identifier(), "john"));
        dotest(h3.key(d1.identifier(), "john", "john password") == d1.key());

        h3.removePassword(d2.identifier(), d2.key(), "please");
        dotest(h3.hasKey(d2.identifier()));
        dotest(h3.key(d2.identifier()) == d2.key());
        dotest(!h3.hasUsername(d2.identifier(), "please"));

        try {
          h3.key(d1.identifier());
          dotest(false /*expected exception*/);
        } catch (const json::WrongType &) {
          // expected
        }

        try {
          h3.hasKey(d3.identifier());
          dotest(false /*expected exception*/);
        } catch (const msg::Exception &) {
          // expected
        }
      }
    }
  } catch (const std::exception &e) {
    printf("FAIL: Exception: %s\n", e.what());
  }
  return 0;
}

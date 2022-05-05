package tau::grammar;

use Exporter 'import';
BEGIN { tau::peg2->import(':all') }


# TODO
# We need an expression language that does prefix dispatch, just like ni.
# We also need north/south default placements for aux data.
# And `:` binding sections.

# TODO: horizontal whitespace delimiters

# TODO: template-oriented parsing (2D regex expressions that macroexpand into
# C++ code)

use constant expr => re qr/.*/, 'e';


use constant pstart => sub
{
  my ($m, $x, $y) = @_;
  return pstream->($m, $x, $y) if $m->at($x, $y) == ord '=';

  my ($sx, $sy, $c) = $m->next($x, $y, 's');
  return undef unless $c eq '=';
  pstream->($m, $sx, $sy);
};

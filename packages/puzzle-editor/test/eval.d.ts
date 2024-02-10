// noinspection SpellCheckingInspection
interface Data {
  id: string;
  contents: string;
  title: string;
  description: string;
  creator: string;
  isPrivate: boolean;
  solves: number;
  upvotes: number;
  creatorName: string;
  createUtc: number;
  voted: number;
}

interface Shape {
  width: number;
  grid: boolean[];
  free: boolean;
  negative: boolean;
}

interface Entity {
  type: number;
  color: number;
  orientation: number | null;
  shape: Shape | null;
  count: number;
  triangleCount: number;
}

interface Storage {
  width: number;
  entity: Entity[];
  symmetry: number;
}
